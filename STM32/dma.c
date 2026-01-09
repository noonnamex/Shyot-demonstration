#include "stm32l476xx.h"
#include "dma.h"
#include "gpio.h"

extern volatile bool dma_busy;
extern uint16_t Buffer[];



/*****************************************************
 *                DMA1 Channel5 IRQ Handler
 ****************************************************/

void DMA1_CH5_IRQHandler(void)
{
    dma_busy = false;

    GPIOA->ODR ^= (1 << 5);
    DMA1->IFCR |= DMA_IFCR_CTCIF5; 			// Clear interrupt flag

    while (!(TIM2->SR & TIM_SR_UIF)); 		// čaká na koniec aktuálneho PWM cyklu
    TIM2->SR &= ~TIM_SR_UIF;          		// zmaž flag

    TIM2->CR1  &= ~TIM_CR1_CEN;      		// Stop timer
}



/*****************************************************
 *    DMA_Channel5 Initialization for TIM2 CH1
 ****************************************************/

void DMA1_Channel5_for_TIM2_CH1_Init(uint16_t *buffer, uint32_t length)
{
    // 1. Povoliť DMA1 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    // 2. Nastaviť DMA request mapping
    DMA1_CSELR->CSELR &= ~(0xF << (4 * (5 - 1)));   // Nulovanie starých registrov pre kanál 5
    DMA1_CSELR->CSELR |=  (0x4 << (4 * (5 - 1)));   // nastav DMA1 Channel 5 na request 4 (TIM2_CH1)

    // 3. Zastaviť DMA pred konfiguráciou
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;

    // 4. Nastaviť adresy
    DMA1_Channel5->CPAR = (uint32_t)&TIM2->CCR1;    // Cieľ: Timer CCR1
    DMA1_Channel5->CMAR = (uint32_t)buffer;         // Zdroj: Buffer
    DMA1_Channel5->CNDTR = length;                  // Počet prenosov

    // 5. Konfigurácia DMA
    DMA1_Channel5->CCR = DMA_CCR_MINC               // Auto-inkrement pamäte
                       | DMA_CCR_DIR                // Pamäť -> periféria
                       | DMA_CCR_TCIE               // Povoli prerušenie po dokončení prenosu
                       | DMA_CCR_PL_1               // Priorita medium
                       | DMA_CCR_MSIZE_0            // 16-bit pamäť
                       | DMA_CCR_PSIZE_1;           // 32-bit periféria

    // 6. Povoliť DMA interrupt
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    // 7. Povoliť DMA request v TIM2 (TIM2 bude žiadať dáta z DMA)
    TIM2->DIER |= TIM_DIER_CC1DE;

    // 8. Spustiť DMA
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}


























