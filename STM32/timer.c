
#include "stm32l476xx.h"
#include "timer.h"



/*****************************************************
 *                Timer2 Initialization
 ****************************************************/

void Timer2_Init(uint32_t prescaler, uint32_t auto_reload) {
    // 1. Enable TIM2 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // 2. Reset TIM2
    RCC->APB1RSTR1 |= RCC_APB1RSTR1_TIM2RST;
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_TIM2RST;

    // 3. Nastaví predeličku a reload register
    TIM2->PSC = prescaler;
    TIM2->ARR = auto_reload;

    // 4. Vynútený update event - okamžite načíta PSC a ARR
    TIM2->EGR = TIM_EGR_UG;

    // 5. Povolí prerušenie, keď časovač dosiahne 0
    TIM2->DIER |= TIM_DIER_UIE;

    // 6. Povolí TIM2 prerušenie v NVIC
    NVIC_EnableIRQ(TIM2_IRQn);

    // 7. Spustí timer
    TIM2->CR1 |= TIM_CR1_CEN;
}



/*****************************************************
 *                PWM Initialization
 ****************************************************/
void Timer2_PWM_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;		// povoliť TIM2 clock

	RCC->APB1RSTR1 |= RCC_APB1RSTR1_TIM2RST;	// resetovať TIM2 - nastavenie reset bitu
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_TIM2RST;	// Zrušiť reset (bit sa vynuluje)

	TIM2->PSC = prescaler;						// SET PRESCALER
	TIM2->ARR = arr;							// Set ARR

	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;				// vymaze nastavenia
	TIM2->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);  	// PWM mode 1 (hodnota 6)

	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;				// hodnota v CCR registri sa zapise z tienoveho registra az pri dalsej PWM periode nie hned
	//TIM2->CR1   |= TIM_CR1_ARPE;				// hodnota v ARR -||-

	TIM2->CCR1 = ccr;							// nastavi sa porovnavacia hodnota

    TIM2->CCER |= TIM_CCER_CC1E;				// povoli sa vystup kanalu 1

    TIM2->EGR = TIM_EGR_UG;						// vygenerovať update event aby sa nacitali prescaler a ARR

    TIM2->CR1 |= TIM_CR1_CEN;					// spustit timer

}



/*****************************************************
 *                Timer Control Functions
 ****************************************************/
void Timer2_Start(void) {
    TIM2->CR1 |= TIM_CR1_CEN;
}

void Timer2_Stop(void) {
    TIM2->CR1 &= ~TIM_CR1_CEN;
}

void Timer2_SetPrescaler(uint32_t prescaler) {
    TIM2->PSC = prescaler;
    TIM2->EGR = TIM_EGR_UG;  // Update prescaler immediately
}

void Timer2_SetAutoReload(uint32_t auto_reload) {
    TIM2->ARR = auto_reload;
}

void Timer2_SetCompareValue(uint32_t ccr) {
    TIM2->CCR1 = ccr;
}


/*****************************************************
 *                Timer2 IRQ Handler
 ****************************************************/
/*
void TIM2_IRQHandler(void){

	 // čakaj, kým prepadne timer (UIF == 1)
	if(TIM2->SR & TIM_SR_UIF){

	// vymaž flag
	TIM2->SR &= ~TIM_SR_UIF;

	// toggle LED
	GPIOA->ODR ^= (1 << 5);
	}

}*/



/*****************************************************
 *                Timer2 Initialization
 ****************************************************/
/*void Timer2_Init(uint32_t prescaler, uint32_t auto_reload){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;		// povoliť hodiny TIM2

	RCC->APB1RSTR1 |= RCC_APB1RSTR1_TIM2RST;	// resetovať TIM2 - nastavenie reset bitu
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_TIM2RST;	// Zrušiť reset (bit sa vynuluje)

	TIM2->PSC = prescaler;						// SET PRESCALER

	TIM2->ARR = auto_reload;					// Set ARR

	TIM2->EGR = TIM_EGR_UG;						// generate update event to load prescaler immediately

	TIM2->DIER |= TIM_DIER_UIE;					// povolit interrupt pri preteceni

	NVIC_EnableIRQ(TIM2_IRQn);					// enable TIM2 interrupts

	TIM2->CR1 |= TIM_CR1_CEN;					// start timer

}
*/


