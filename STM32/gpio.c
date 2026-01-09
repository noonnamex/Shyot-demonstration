#include "stm32l476xx.h"
#include "gpio.h"

/*****************************************************
 *                GPIO PA0 Initialization
 * (PWM output for WS2812 LEDs - TIM2_CH1 on PA0)
 ****************************************************/
void GPIO_PA0_Init(void) {
    // 1. Povoliť hodiny
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // 2. Nastavenie PA0 ako alternatívnu funkciu
    GPIOA->MODER &= ~(3U << (0 * 2));          // nulovanie bitov 0-1
    GPIOA->MODER |=  (2U << (0 * 2));          // režim alternatívnej funkcie (10bin)

    // 3. Nastav alternatívnu funkciu na AF1 (TIM2_CH1)
    GPIOA->AFR[0] &= ~(0xF << (0 * 4));        // Vynuluj AF bity pre PA0 (bity 0-3)
    GPIOA->AFR[0] |=  (1U << (0 * 4));         // AF1 = TIM2_CH1 (TIM2 Kanál 1)

    // 4. Configure output type and speed
    GPIOA->OTYPER &= ~(1U << 0);               // Push-pull output
    GPIOA->OSPEEDR |= (2U << (0 * 2));         // High speed
    GPIOA->PUPDR &= ~(3U << (0 * 2));          // No pull-up/pull-down
}




/*****************************************************
 *          GPIO internal LED Initialization
 * (Debug LED on PA5)
 ****************************************************/
void GPIO_led_init(void) {
    // 1. Zapni clock pre GPIOA
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // 2. Nastav PA5 ako všeobecný výstup
    GPIOA->MODER &= ~(3U << (5 * 2));          // Vynuluj módu bity pre PA5 (bity 10-11)
    GPIOA->MODER |=  (1U << (5 * 2));          // Režim všeobecného výstupu (01)

    // 3. Configure output type and speed
    GPIOA->OTYPER &= ~(1U << 5);               // Push-pull output
    GPIOA->OSPEEDR |= (2U << (5 * 2));         // High speed
    GPIOA->PUPDR &= ~(3U << (5 * 2));          // No pull-up/pull-down

    // 4. Na začiatku je LED vypnuté
    GPIOA->ODR &= ~(1U << 5);
}


// Toggle debug LED
void GPIO_Toggle_LED(void) {
    GPIOA->ODR ^= (1U << 5);
}


// Set debug LED state
void GPIO_Set_LED(bool state) {
    if (state) {
        GPIOA->ODR |= (1U << 5);				// Nastaví bit 5 na 1 (HIGH)
    } else {
        GPIOA->ODR &= ~(1U << 5);				// Nastaví bit 5 na 1 (LOW)
    }
}

// Read debug LED state
bool GPIO_Get_LED(void) {
    return (GPIOA->ODR & (1U << 5)) != 0;		// Testuje bit 5: ak ≠0 → true, inak false
}



















