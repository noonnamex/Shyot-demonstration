#ifndef EFFECT_H
#define EFFECT_H

#include <stdint.h>
#include <stdbool.h>

// Definicie
#define LED_COUNT 699
#define BIT_PER_LED 24

// Deklaracie premennych
extern volatile float restriction;
extern uint8_t brightness;
extern volatile uint16_t everyNth;

/// ladenie efektov
//extern volatile uint8_t a_fire;
//extern volatile uint8_t b_fire;


// Deklaracie funkcii
void HSV_to_RGB(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b);

void RAINBOW(void);
void SET_LED(uint8_t r, uint8_t g, uint8_t b, uint16_t index);
void SET_ALL(uint8_t r, uint8_t g, uint8_t b);
void FIRE(uint8_t order);

#endif // EFFECT_H
