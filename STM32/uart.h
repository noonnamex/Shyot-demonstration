#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define UART_BUFFER_SIZE 128  // Veľkosť bufferu pre UART dáta

// UART globálne premenné
extern volatile char uart_rx_buffer[UART_BUFFER_SIZE];
extern volatile uint16_t uart_rx_index;
extern volatile bool uart_buffer_ready;
extern volatile uint16_t uart_total_bytes;

// SET,program,jas,rýchlosť,R,G,B,parameter
extern volatile char default_status[];

// Globálne premenné pre parsovanie
extern bool request_show;
extern uint8_t current_program;
extern uint8_t program;
extern uint8_t brightness;
extern uint8_t speed;
extern uint8_t red;
extern uint8_t green;
extern uint8_t blue;
extern uint8_t parameter;

// UART inicializácia
void UART_GPIO_Init(void);
void UART_Init(void);
void UART_RX_Interrupt_Init(void);
void UART_Init_All(void);

// UART funkcie
void UART_Transmit(uint8_t data);
void UART_Transmit_String(const char *str);
uint8_t UART_Receive(void);

// Buffer funkcie
uint16_t get_uart_bytes_received(void);
uint8_t* get_uart_buffer(void);
uint16_t get_uart_buffer_length(void);
void clear_uart_buffer(void);
uint8_t get_uart_byte(uint16_t index);

// Parsovanie príkazov
void parse_uart_command(char* cmd);
void update_status_message(void);

// IRQ Handler
void USART1_IRQHandler(void);

#endif // UART_H
