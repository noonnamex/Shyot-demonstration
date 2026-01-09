#include "stm32l476xx.h"
#include "clock.h"



/*****************************************************
 *             FPU - floating point unit
 ****************************************************/

void SystemInit(void) {
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));    // Povoliť FPU (Coprocessor 10 a 11)
}



/*****************************************************
 *                System clock Initialization
 ****************************************************/

void SystemClock_Config(void) {
    // 1. Povoliť HSI (High-Speed Internal 16 MHz RC oscilátor)
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    // 2. Nastaviť Flash wait states na 4 WS (pre 80 MHz pri Range 1)
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS; 				// 4 wait states pre 80 MHz
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_4WS);

    // 3. Vypnúť PLL pred konfiguráciou
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY); 					// počkaj kým PLL zhasne

    // 4. Nastaviť zdroj PLL na HSI
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;             	// Zmaž predchádzajúci zdroj
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;          	// HSI ako zdroj PLL

    // 5. Vymaž PLLM, PLLN, PLLR a nastav nové hodnoty:
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLR);

    // PLLM = 1 (delí HSI 16 MHz / 1 = 16 MHz)
    RCC->PLLCFGR |= (0 << RCC_PLLCFGR_PLLM_Pos);     	// PLLM = 1

    // PLLN = 10 → 16 MHz * 10 = 160 MHz VCO
    RCC->PLLCFGR |= (10 << RCC_PLLCFGR_PLLN_Pos);

    // PLLR = 2 → 160 MHz / 2 = 80 MHz
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;              	// Enable PLLR output
    RCC->PLLCFGR |= (0b00 << RCC_PLLCFGR_PLLR_Pos);  	// PLLR = 2

    // 6. Povoliť PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 7. Nastaviť AHB, APB1 a APB2 prescalery na /1
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    // Všetky zbernice budú bežať na plných 80 MHz

    // 8. Prepnúť systémový clock na PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
