#include "uart.h"

#include "stm32l476xx.h"
#include <stdlib.h>



/*****************************************************
 *                		UART premenné
 ****************************************************/

volatile char uart_rx_buffer[UART_BUFFER_SIZE];  	// Buffer pre prijaté dáta
volatile uint16_t uart_rx_index = 0;             	// Index pre buffer

// SET,program,jas,rýchlosť,R,G,B,parameter
volatile char default_status[] = "SET,1,50,50,255,255,000,0\n";

uint8_t current_program = 0;						// program ktorý sa má zobrazovať

// premenné z ESP
uint8_t program;
uint8_t brightness;
uint8_t speed;
uint8_t red;
uint8_t green;
uint8_t blue;
uint8_t parameter;



/*****************************************************
 *                	UART Initialization
 ****************************************************/

void UART_GPIO_Init(void) {
    // 1. Povoliť hodiny pre GPIOA
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // 2. Nastaviť PA9 ako alternatívnu funkciu (TX) s vysokou rýchlosťou
    GPIOA->MODER &= ~(GPIO_MODER_MODE9);  // Clear bits
    GPIOA->MODER |= (2U << (9 * 2));      // Alternate function mode

    // AF7 pre USART1_TX na PA9
    GPIOA->AFR[1] &= ~(0xF << ((9 - 8) * 4));
    GPIOA->AFR[1] |= (7U << ((9 - 8) * 4));

    GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;   // Push-pull
    GPIOA->OSPEEDR |= (3U << (9 * 2));   // Very high speed
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9); // No pull-up/pull-down

    // 3. Nastaviť PA10 ako alternatívnu funkciu (RX)
    GPIOA->MODER &= ~(GPIO_MODER_MODE10); // Clear bits
    GPIOA->MODER |= (2U << (10 * 2));     // Alternate function mode

    // AF7 pre USART1_RX na PA10
    GPIOA->AFR[1] &= ~(0xF << ((10 - 8) * 4));
    GPIOA->AFR[1] |= (7U << ((10 - 8) * 4));

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD10);
    GPIOA->PUPDR |= (1U << (10 * 2));     // Pull-up
}

void UART_Init(void) {
    // 1. Povoliť hodiny pre USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // 2. Vypnúť USART1 pred konfiguráciou
    USART1->CR1 &= ~USART_CR1_UE;

    // 3. Nastaviť baud rate
    USART1->BRR = 0x208D;  // 9600 baud na 80 MHz

    // 4. Konfigurácia
    USART1->CR1 &= ~USART_CR1_M;     // 8 data bits
    USART1->CR1 &= ~USART_CR1_PCE;   // No parity
    USART1->CR2 &= ~USART_CR2_STOP;  // 1 stop bit

    // 5. Povoliť TX a RX
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;

    // 6. DÔLEŽITÉ: Povoliť RX interrupt
    USART1->CR1 |= USART_CR1_RXNEIE;  // RX not empty interrupt

    // 7. Povoliť USART1
    USART1->CR1 |= USART_CR1_UE;

    // 8. Vyčisti RXNE flag
    USART1->RDR;  // Prečítaj aby sa vyčistil RXNE
}


void UART_RX_Interrupt_Init(void) {
    // 1. Vyčisti pending flag
    NVIC_ClearPendingIRQ(USART1_IRQn);

    // 2. Nastav prioritu (nižšie číslo = vyššia priorita)
    NVIC_SetPriority(USART1_IRQn, 2);

    // 3. Povoli interrupt
    NVIC_EnableIRQ(USART1_IRQn);
}


void UART_Init_All(void) {
    // 1. Inicializácia GPIO pinov
    UART_GPIO_Init();

    // 2. Inicializácia USART1
    UART_Init();

    // 3. Inicializácia prerušenia
    UART_RX_Interrupt_Init();

    // 4. Povoliť globálne prerušenia
    __enable_irq();
}



/*****************************************************
 *                	UART Functions
 ****************************************************/

void UART_Transmit(uint8_t data) {
    // Čakať kým TX buffer nie je prázdny
    while (!(USART1->ISR & USART_ISR_TXE));

    // Zapísať dáta
    USART1->TDR = data;

    // Čakať kým sa prenesie
    while (!(USART1->ISR & USART_ISR_TC));
}


void UART_Transmit_String(const char *str) {
    while (*str) {
        UART_Transmit(*str++);
    }
}

// Funkcia na prijatie bajtu
uint8_t UART_Receive(void) {
    while (!(USART1->ISR & USART_ISR_RXNE));  // Čakaj na dáta
    return (uint8_t)USART1->RDR;              // Vráť prijatý bajt
}



/*****************************************************
 *                	Buffer Functions
 ****************************************************/

// Vráti počet prijatých bajtov
uint16_t get_uart_bytes_received(void) {
    return uart_total_bytes;
}

// Vráti aktuálny obsah bufferu
uint8_t* get_uart_buffer(void) {
    return (uint8_t*)uart_rx_buffer;
}

// Vráti aktuálnu dĺžku bufferu
uint16_t get_uart_buffer_length(void) {
    return uart_rx_index;
}

// Vymaže buffer
void clear_uart_buffer(void) {
    uart_rx_index = 0;
    for(int i = 0; i < UART_BUFFER_SIZE; i++) {
        uart_rx_buffer[i] = 0;
    }
}

// Vráti konkrétny bajt z bufferu
uint8_t get_uart_byte(uint16_t index) {
    if (index < uart_rx_index) {
        return uart_rx_buffer[index];
    }
    return 0;
}



/*****************************************************
 *                	Command Parsing
 ****************************************************/

void parse_uart_command(char* cmd) {
    char *message;

    // Očakávame SET
    message = strtok(cmd, ",");
    if (!message || strcmp(message, "SET") != 0) return;

    // program
    message = strtok(NULL, ",");
    if (!message) return;
    current_program = atoi(message);

    // jas
    message = strtok(NULL, ",");
    if (!message) return;
    brightness = atoi(message);

    // rýchlosť
    message = strtok(NULL, ",");
    if (!message) return;
    speed = atoi(message);

    // R
    message = strtok(NULL, ",");
    if (!message) return;
    red = atoi(message);

    // G
    message = strtok(NULL, ",");
    if (!message) return;
    green = atoi(message);

    // B
    message = strtok(NULL, ",");
    if (!message) return;
    blue = atoi(message);

    // Parameter
    message = strtok(NULL, ",");
    if (!message) return;
    parameter = atoi(message);
}



/*****************************************************
 *                	IRQ Handler
 ****************************************************/

void USART1_IRQHandler(void) {

	// 1. SKONTROLUJ či prišli nové dáta (RXNE = Receive Not Empty)
    if (USART1->ISR & USART_ISR_RXNE) {
	uint8_t received = (uint8_t)(USART1->RDR);				// Načítaj prijatý znak z registra RDR (Receive Data Register)

		// 2. IGNORUJ \r (carriage return) - Windows používa \r\n, Linux \n
        if (received == '\r') return;

        // 3. AK PRIŠIEL \n (newline) - koniec správy
        if (received == '\n') {
            uart_rx_buffer[uart_rx_index] = '\0';  			// Pridaj null terminator na koniec reťazca

            // Spracuj správu ak nie je prázdna
            if (uart_rx_index > 0) {
                parse_uart_command((char*)uart_rx_buffer);	// Volaj funkciu na spracovanie
            }
            uart_rx_index = 0;								// Resetuj index pre ďalšiu správu
            return;
        }

        // 4. ULOŽ ZNAK do bufferu (ak je miesto)
        if (uart_rx_index < UART_BUFFER_SIZE - 1) {
            uart_rx_buffer[uart_rx_index++] = received;
        }
    }

    // Vyčisti chybové flagy
    if (USART1->ISR & USART_ISR_ORE) USART1->ICR |= USART_ICR_ORECF;
    if (USART1->ISR & USART_ISR_FE) USART1->ICR |= USART_ICR_FECF;
    if (USART1->ISR & USART_ISR_NE) USART1->ICR |= USART_ICR_NCF;
}




















