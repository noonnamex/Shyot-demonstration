
/*****************************************************
 *                	ADC2 MERANIE PRUDU
 ****************************************************/

/*
void ADC2_Init(void) {
    // 1. ADC clock source
    RCC->CCIPR &= ~RCC_CCIPR_ADCSEL;
    RCC->CCIPR |= (0b11 << RCC_CCIPR_ADCSEL_Pos); // SYSCLK
    ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;
    ADC123_COMMON->CCR |= (0b01 << ADC_CCR_CKMODE_Pos); // Sync mode /1

    DELAY(1);

    // 2. GPIOC PC1 = analog, no pull
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;        // Povoliť clock na GPIOC
    GPIOC->MODER &= ~(3U << (1 * 2));           // Vyčistiť bity pre PC1
    GPIOC->MODER |=  (3U << (1 * 2));           // PC1 analog mode
    GPIOC->PUPDR &= ~(3U << (1 * 2));           // PC1 no pull

    // 3. Enable ADC clock
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    (void)RCC->AHB2ENR;

    // 4. Disable ADC if already enabled
    if (ADC2->CR & ADC_CR_ADEN) {
        ADC2->CR |= ADC_CR_ADDIS;
        while (ADC2->CR & ADC_CR_ADEN);
    }

    // 5. Enable voltage regulator
    ADC2->CR &= ~ADC_CR_ADVREGEN;
    ADC2->CR |= (0b01 << ADC_CR_ADVREGEN_Pos);
    DELAY(30); // stabilizácia 30ms

    // 6. Calibrate ADC
    ADC2->CR |= ADC_CR_ADCAL;
    while (ADC2->CR & ADC_CR_ADCAL);

    // 7. Enable ADC
    ADC2->CR |= ADC_CR_ADEN;
    while (!(ADC2->ISR & ADC_ISR_ADRDY));

    // 8. Sampling time (max pre stabilitu)
    ADC2->SMPR1 &= ~(7U << ADC_SMPR1_SMP2_Pos);
    ADC2->SMPR1 |=  (7U << ADC_SMPR1_SMP2_Pos); // 640.5 cycles pre kanál 2

    // 9. Channel sequence
    ADC2->SQR1 = 0;
    ADC2->SQR1 |= (2 << ADC_SQR1_SQ1_Pos); // channel 2 (PC1)
}


uint16_t ADC2_Read(void) {
    adc_started = false;
    adc_finished = false;
    adc_timeout = false;

    // Reset EOC flag pred meraním
    ADC2->ISR |= ADC_ISR_EOC;

    // Spusti konverziu
    ADC2->CR |= ADC_CR_ADSTART;
    adc_started = true;   // Indikuj, že meranie bolo spustené

    uint32_t timeout = 1000000;
    while (!(ADC2->ISR & ADC_ISR_EOC) && timeout--) {
        __NOP();
    }

    if (timeout == 0) {
        adc_timeout = true;   // Indikuj, že meranie zlyhalo (timeout)
        return 0xFFFF;
    }

    adc_finished = true;    // Meranie úspešne skončilo
    adc_value_debug = ADC2->DR;  // Ulož nameranú hodnotu pre debug

    return adc_value_debug;
}
*/

/*
void ADC2_Init(void) {
    // Enable GPIOC + ADC clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_ADCEN;

    // PC1 analog
    GPIOC->MODER |=  (3U << (1*2));
    GPIOC->PUPDR &= ~(3U << (1*2));

    // ADC clock = async
    RCC->CCIPR &= ~RCC_CCIPR_ADCSEL;
    RCC->CCIPR |= (0b00 << RCC_CCIPR_ADCSEL_Pos); // HSI16

    ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE; // async mode

    // Disable ADC if running
    if (ADC2->CR & ADC_CR_ADEN) {
        ADC2->CR |= ADC_CR_ADDIS;
        while (ADC2->CR & ADC_CR_ADEN);
    }

    // Voltage regulator enable (ADVREGEN = 01)
    ADC2->CR &= ~ADC_CR_ADVREGEN;          // vyčisti ADVREGEN bity
    ADC2->CR |=  (1U << ADC_CR_ADVREGEN_Pos); // nastav "enabled"
    for (volatile int i = 0; i < 100000; i++); // krátka pauza


    // Calibration
    ADC2->CR |= ADC_CR_ADCAL;
   // while (ADC2->CR & ADC_CR_ADCAL);

    // Enable ADC
    ADC2->CR |= ADC_CR_ADEN;
    //while (!(ADC2->ISR & ADC_ISR_ADRDY));

    // Sampling time
    ADC2->SMPR1 &= ~(7U << ADC_SMPR1_SMP2_Pos);
    ADC2->SMPR1 |=  (7U << ADC_SMPR1_SMP2_Pos);

    // Sequence: channel 2 (PC1)
    ADC2->SQR1 = (2 << ADC_SQR1_SQ1_Pos);
}

uint16_t ADC2_Read(void) {
    ADC2->ISR |= ADC_ISR_EOC; // clear EOC

    ADC2->CR |= ADC_CR_ADSTART;
    //while (!(ADC2->ISR & ADC_ISR_EOC));

    return (uint16_t)ADC2->DR;
}
*/

//void ADC2_Init(void) {
/*
    // 0. Povoliť hodiny pre ADC a GPIOC
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // 1. GPIO PC1 ako analog input
    GPIOC->MODER &= ~(3U << (1 * 2));
    GPIOC->MODER |=  (3U << (1 * 2));
    GPIOC->PUPDR &= ~(3U << (1 * 2));   // no pull-up / pull-down

    // 2. Vypni deep power down
    ADC2->CR &= ~ADC_CR_DEEPPWD;

    // 3. Zapni ADC voltage regulator
    ADC2->CR |= ADC_CR_ADVREGEN;
    for (volatile int i = 0; i < 3000; i++); // malá stabilizačná pauza

    // 4. Kalibrácia (ADC musí byť vypnutý – ADEN=0)
    /*if (ADC2->CR & ADC_CR_ADEN) {
        ADC2->CR |= ADC_CR_ADDIS;              // disable
        while (ADC2->CR & ADC_CR_ADEN);        // počkaj na vypnutie
    }
    ADC2->CR &= ~ADC_CR_ADCALDIF;              // single-ended
    ADC2->CR |= ADC_CR_ADCAL;                  // start calibration
    while (ADC2->CR & ADC_CR_ADCAL);           // počkaj na koniec
*/
    // 5. Nastav sampling time a sekvenciu
    // Sampling time pre kanál 2 = 19.5 ADC cycles (ako HAL)
  /*  ADC2->SMPR1 &= ~(7U << ADC_SMPR1_SMP2_Pos);
    ADC2->SMPR1 |=  (4U << ADC_SMPR1_SMP2_Pos);

    // Sekvencia: 1 konverzia, prvý slot = kanál 2
    ADC2->SQR1 &= ~ADC_SQR1_L;                         // length = 0 → 1 konverzia
    ADC2->SQR1 &= ~(ADC_SQR1_SQ1_Msk);
    ADC2->SQR1 |=  (2U << ADC_SQR1_SQ1_Pos);           // channel 2

    // 6. Nastavenie CFGR (continuous mode, bez DMA)
    ADC2->CFGR = 0;                  // všetko default
    ADC2->CFGR |= ADC_CFGR_CONT;     // continuous mode

    // 7. Vyčisti príznak ADRDY
    ADC2->ISR |= ADC_ISR_ADRDY;

    // 8. Povoliť ADC
    ADC2->CR |= ADC_CR_ADEN;
    while (!(ADC2->ISR & ADC_ISR_ADRDY));   // čakaj na pripravenosť

    // 9. Spusti konverziu
    ADC2->CR |= ADC_CR_ADSTART;
}


uint16_t ADC2_Read(void) {
	/*
    // Začni konverziu
    ADC2->CR |= ADC_CR_ADSTART;

    // Čakaj na koniec konverzie
    while (!(ADC2->ISR & ADC_ISR_EOC));

    // Vyčisti flag EOC
    ADC2->ISR |= ADC_ISR_EOC;

    // Vráť výsledok
    return (uint16_t)(ADC2->DR);
	return 0;
}
*/


