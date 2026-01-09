#include "effect.h"
#include "timer.h"
#include "gpio.h"
#include <math.h>
#include <stdlib.h>

extern volatile float restriction;
extern volatile uint16_t everyNth;
extern uint16_t LED[LED_COUNT][BIT_PER_LED];
extern void Show(void);
extern void DELAY(uint32_t ms);
#define HIGH 58
#define LOW 32

/// ladenie efektov
//volatile uint8_t a_fire = 1;
//volatile uint8_t b_fire = 1;



/*****************************************************
 *    			RAINBOW ANIMATION
 ****************************************************/

/// Výpočet odtieňov
void HSV_to_RGB(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b) {
    float c = v * s;
    float x = c * (1 - fabsf(fmodf(h / 60.0f, 2) - 1));
    float m = v - c;

    float r_, g_, b_;

    if (h < 60) {
        r_ = c; g_ = x; b_ = 0;
    } else if (h < 120) {
        r_ = x; g_ = c; b_ = 0;
    } else if (h < 180) {
        r_ = 0; g_ = c; b_ = x;
    } else if (h < 240) {
        r_ = 0; g_ = x; b_ = c;
    } else if (h < 300) {
        r_ = x; g_ = 0; b_ = c;
    } else {
        r_ = c; g_ = 0; b_ = x;
    }

    *r = (uint8_t)((r_ + m) * 255);
    *g = (uint8_t)((g_ + m) * 255);
    *b = (uint8_t)((b_ + m) * 255);
}


void RAINBOW(void) {
    static uint16_t shift = 0;    					// posun celej dúhy
    uint8_t r, g, b;								// deklarácie farieb

    for (int i = 0; i < LED_COUNT - 10; i++) {
        // Výpočet odtieňa (hue) pre aktuálnu LED:
        // 1. (i + shift) - kombinácia pozície LED a globálneho posunu
        // 2. / (LED_COUNT - 10) - normalizácia na rozsah 0-1
        // 3. * 360.0f - rozšírenie na 0-360° (plný farebný kruh)
        // 4. fmodf(..., 360.0f) - zaistenie cyklického opakovania (360° → 0°)
        float hue = fmodf(((float)(i + shift) / (LED_COUNT - 10)) * 360.0f, 360.0f);

        // Konverzia HSV (hue, plná sýtosť=1.0, plný jas=1.0) na RGB
        HSV_to_RGB(hue, 1.0f, 1.0f, &r, &g, &b);

        // nastavenie farby pre jednotlivé ledky
        SET_LED(r, g, b, i);
    }

    // zobrazenie a posun
    Show();
    shift = (shift + 1) % (LED_COUNT - 10);    // nekonečné opakovanie

    // pauza - rýchlosť
    DELAY(30);
}


/*****************************************************
 *    			NASTAVÍ FARBU 1 LED
 ****************************************************/

void SET_LED(uint8_t r, uint8_t g, uint8_t b, uint16_t index) {

    // Prevod brightness (0-100) na desatinné číslo (0.0-1.0)
    float brightness_factor = (float)brightness / 100.0f;


    // vynechávanie každej n-tej ledky
    if (index % everyNth != 0) {  // Zapni len každú N-tu LEDku - úspora
        g = 0; r = 0; b = 0;
    }

    // + 0.5f je pre zaokruhlovanie, restriction pre obmedzenie jasu, brightness využitie možného jasu
    r = (uint8_t)(r * restriction * brightness_factor + 0.5f);
    g = (uint8_t)(g * restriction * brightness_factor+ 0.5f);
    b = (uint8_t)(b * restriction * brightness_factor+ 0.5f);

    // ledky ktoré robia problémy sú vypnuté
    if ((index >= (LED_COUNT - 10)) || (index < 0)) return;
    if (index == 175) { g = 0; r = 0; b = 0; }

    // prvých 10 sa neprepisuje lebo sú súčasť reset signálu
    index += 10;

    // farby sa uložia do poľa
    for (int bit = 0; bit < 8; bit++) {
        LED[index][bit]       = (r & (1 << (7 - bit))) ? HIGH : LOW;
        LED[index][8 + bit]   = (g & (1 << (7 - bit))) ? HIGH : LOW;
        LED[index][16 + bit]  = (b & (1 << (7 - bit))) ? HIGH : LOW;
    }
}


/*****************************************************
 *    			NASTAVÍ FARBU VŠETKÝCH LEDIEK
 ****************************************************/

void SET_ALL(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < LED_COUNT - 10; i++) {
        SET_LED(r, g, b, i);
    }
}


/*****************************************************
 *    				OHEŇ ANIMÁCIA
 ****************************************************/

void FIRE(uint8_t order) {

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint16_t led = rand() % LED_COUNT;

	// ochrana proti neplatným vstupom
    if (order < 1 || order > 6) {
        order = 1;
    }

    // prepínač poradia farieb
    switch (order) {

    /*
  		x - main color
		o - sub color
		- - off

        1.  x   o   -	RG
        2.  -   x   o	GB
        3.  o   -   x	BR
        4.  x   -   o	RB
        5.  o   x   -	GR
        6.  -   o   x	BG
    */

        case 1:
        	// výpočet farieb
            r = 200 + rand() % 56;	// rozsah: 200-255 (200 + 0..55)
            g = rand() % 50;		// rozsah: 0-49
            b = 0;					// rozsah: 0
            break;

        case 2:
            r = 0;
            g = 200 + rand() % 1;	// vždy 200
            b = rand() % 100;		// 0-99
            break;

        case 3:
            r = rand() % 150;
            g = 0;
            b = 200 + rand() % 50;
            break;

        case 4:
            r = 200 + rand() % 20;
            g = 0;
            b = rand() % 30;
            break;

        case 5:
            r = rand() % 255;
            g = 200 + rand() % 50;
            b = 0;
            break;

        case 6:
            r = 0;
            g = rand() % 255;
            b = 200 + rand() % 1;
            break;

        default:
            break;
    }

    // nastavenie farieb s rozhodením
    SET_LED(r, g, b, led + 30);
    SET_LED(r, g, b, led + 31);
    SET_LED(r, g, b, led + 32);

    SET_LED(r, g, b, led + 160);
    SET_LED(r, g, b, led + 161);
    SET_LED(r, g, b, led + 162);

    SET_LED(r, g, b, led + 200);
    SET_LED(r, g, b, led + 201);
    SET_LED(r, g, b, led + 202);

    SET_LED(r, g, b, led + 512);
    SET_LED(r, g, b, led + 513);
    SET_LED(r, g, b, led + 514);

    SET_LED(r, g, b, led - 90);
    SET_LED(r, g, b, led - 89);
    SET_LED(r, g, b, led - 88);

    // zobrazenie efektu
    Show();
}























