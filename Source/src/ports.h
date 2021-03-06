#include "stm8s.h"
#include "stm8s_it.h" /* SDCC patch: required by SDCC for interrupts */

#define LED_GPIO_PORT (GPIOA)
#define LED_GPIO_PIN (GPIO_PIN_2)

#define OUT_A_M_PORT (GPIOC)
#define OUT_A_M_PIN (GPIO_PIN_2)
#define OUT_A_P_PORT (GPIOC)
#define OUT_A_P_PIN (GPIO_PIN_4)
#define OUT_B_M_PORT (GPIOE)
#define OUT_B_M_PIN (GPIO_PIN_5)
#define OUT_B_P_PORT (GPIOC)
#define OUT_B_P_PIN (GPIO_PIN_1)

#define PUL_PORT (GPIOC)
#define PUL_PIN (GPIO_PIN_5)
#define DIR_PORT (GPIOD)
#define DIR_PIN (GPIO_PIN_3)
#define ENA_PORT (GPIOD)
#define ENA_PIN (GPIO_PIN_2)

#define SW1_PORT (GPIOD)
#define SW1_PIN (GPIO_PIN_5)
#define SW2_PORT (GPIOD)
#define SW2_PIN (GPIO_PIN_6)
#define SW3_PORT (GPIOD)
#define SW3_PIN (GPIO_PIN_7)
#define SW4_PORT (GPIOA)
#define SW4_PIN (GPIO_PIN_1)
#define SW5_PORT (GPIOB)
#define SW5_PIN (GPIO_PIN_4)
#define SW6_PORT (GPIOB)
#define SW6_PIN (GPIO_PIN_5)
#define SW7_PORT (GPIOB)
#define SW7_PIN (GPIO_PIN_6)
#define SW8_PORT (GPIOB)
#define SW8_PIN (GPIO_PIN_7)


#define CurrentA_PORT (GPIOB)
#define CurrentA_PIN (GPIO_PIN_1)
#define CurrentB_PORT (GPIOB)
#define CurrentB_PIN (GPIO_PIN_0)
#define PowerControl_PORT (GPIOB)
#define PowerControl_PIN (GPIO_PIN_2)