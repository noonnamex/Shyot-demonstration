/*****************************************************
 *                Includes a Definície
 ****************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32l476xx.h"

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "timer.h"
#include "uart.h"
#include "clock.h"
#include "gpio.h"
#include "dma.h"
#include "effect.h"
//#include "adc.h"


/*****************************************************
 *                  Nastavenie ARR
 ****************************************************/
#define F_CPU        80000000U   		// System clock in Hz
#define TIM_FREQ     8000U      		// Timer tick frequency Hz
#define DESIRED_FREQ 100U				// Desired interrupt frequency Hz

#define PSC_value    0 					// ((F_CPU / TIM_FREQ) - 1)
#define ARR_value    99      			// ((TIM_FREQ / DESIRED_FREQ) - 1)

// f = F_CPU / ((PSC_value + 1) * (ARR_value + 1))



/*****************************************************
 *          Nastavenie parametrov lediek
 ****************************************************/

#define LED_COUNT 699	// pôvodne 730 			// 10 - reset leds, 21 odpálených
#define BIT_PER_LED 24							// RGB 8 bitov
#define BUFFER_SIZE (LED_COUNT * BIT_PER_LED)	// veľkosť buffra


#define HIGH 58 // 70							// nastavenie dĺžky PWM pulzu HIGH
#define LOW 32  // 30							// nastavenie dĺžky PWM pulzu LOW



/*****************************************************
 *              Deklaracia polí lediek
 ****************************************************/

uint16_t LED[LED_COUNT][BIT_PER_LED];			// LED[1][23xHIGH]
uint16_t Buffer[BUFFER_SIZE];					// Buffer do ktorého sa nakopírujú dáta na odoslanie


 /*****************************************************
  *              Deklaracia premennych
  ****************************************************/
volatile float restriction = 0.30f; //0.392f; 	// vynasobi % z plneho jasu pre obmedzenie prúdu


volatile bool dma_busy = false;					// dma je busy ak vysiela
volatile uint32_t msTicks = 0;					// časovanie výpočtu efektov napr rýchlosť posunu RAINBOW

volatile uint16_t everyNth = 1;					// Zvýšenie jasu ak sa zasvieti menej lediek

// Premenne pre ADC
/*volatile uint8_t desired_sampling = 0;

volatile bool adc_started = false;
volatile bool adc_finished = false;
volatile bool adc_timeout = false;
volatile uint16_t adc_value_debug = 0;*/


/*****************************************************
 *                 Použité funkcie
 ****************************************************/

//void ADC2_Init(void);
//uint16_t ADC2_Read(void);

void SysTick_Handler(void);
void SysTick_Init(void);
void DELAY(uint32_t ms);

void fillBufferFromLED(uint16_t *buf);
void Show();


/*****************************************************
*                	 Main function
 ****************************************************/

int main(void)
{

	SystemClock_Config();
	SystemInit();
	SysTick_Init();

	srand(msTicks);

	GPIO_PA0_Init();							// GIPO vysielanie pulzov pre ledky
	GPIO_led_init();

	//ADC2_Init();

	//Timer2_Init(PSC_value, ARR_value);
    Timer2_PWM_Init(PSC_value, ARR_value, 0);	// Timer pre generovanie pwm pulzov

    // na začiatku je všetko vypnuté
    SET_ALL(0,0,0);
    Show();


    UART_Init_All();							// inicializácia UART


/*****************************************************
 *                		Main Loop
 ****************************************************/

	/// podľa čísla programu sa vykonáva príslušný efekt
	while (1)
	{
		switch(current_program){

			case 1:
				SET_ALL(green, red, blue);
				Show();
				current_program = 0;
			break;


			case 2:
				FIRE(parameter);
			break;


			case 3:
				RAINBOW();
			break;


			default:
			break;
		}
	}
}




// === Skopíruje obsah LED[][] do Buffer[] ===
void fillBufferFromLED(uint16_t *buf) {
    for (int i = 0; i < LED_COUNT; i++) {
        for (int j = 0; j < BIT_PER_LED; j++) {
            buf[i * BIT_PER_LED + j] = LED[i][j];
        }
    }
    for (int i = 0; i < 23; i++){
    buf[17520-i] = 0;

    }
}

// === Spustí DMA prenos ===
void Show() {

	while (dma_busy);				// čakanie na dokončenie prenosu

	fillBufferFromLED(Buffer);		// skopirovanie dát do buffra
	dma_busy = true;				// flag vysielania

	TIM2->CCER |= TIM_CCER_CC1E;   	// zapne výstup CH1
	TIM2->CR1  |= TIM_CR1_CEN;     	// spustí timer

	DMA1_Channel5_for_TIM2_CH1_Init(Buffer, BUFFER_SIZE);	// spustenie prenosu

}



/*****************************************************
 *                	DELAY CASOVANIE
 ****************************************************/

void SysTick_Handler(void) {
    msTicks++;										// zvýšenie času v ms
}

void SysTick_Init(void) {
    SysTick->LOAD = (80000000 / 1000) - 1; 			// Nastavenie intervalu prerušenia: 1ms pri 80MHz clock
    SysTick->VAL = 0;								// Vynulovanie aktuálnej hodnoty časovača
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 	// Zdroj hodín = systémové hodiny (80MHz)
    		SysTick_CTRL_TICKINT_Msk | 				// Povoliť prerušenia
			SysTick_CTRL_ENABLE_Msk;				// Zapnúť časovač
}

void DELAY(uint32_t ms) {
    uint32_t start = msTicks;						// Zapamätaj si aktuálny čas
    while ((msTicks - start) < ms) {;}				// Kým neuplynú 'ms' milisekúnd

}










