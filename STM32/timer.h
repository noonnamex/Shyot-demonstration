#ifndef TIMER_H
#define TIMER_H


#include <stdint.h>


void Timer2_Init(uint32_t prescaler, uint32_t auto_reload);
void Timer2_PWM_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);


void Timer2_Start(void);
void Timer2_Stop(void);
void Timer2_SetPrescaler(uint32_t prescaler);
void Timer2_SetAutoReload(uint32_t auto_reload);
void Timer2_SetCompareValue(uint32_t ccr);


void TIM2_IRQHandler(void);


#endif // TIMER_H
