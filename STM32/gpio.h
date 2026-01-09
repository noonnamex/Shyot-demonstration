#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

// Function declarations
void GPIO_PA0_Init(void);
void GPIO_led_init(void);
void GPIO_Toggle_LED(void);
void GPIO_Set_LED(bool state);
bool GPIO_Get_LED(void);

#endif /* GPIO_H */
