#ifndef DMA_H
#define DMA_H


#include <stdint.h>
#include <stdbool.h>


extern volatile bool dma_busy;
extern uint16_t Buffer[];


void DMA1_Channel5_for_TIM2_CH1_Init(uint16_t *buffer, uint32_t length);
void DMA1_CH5_IRQHandler(void);


#endif /* DMA_H */
