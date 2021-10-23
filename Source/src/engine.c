#include "main.h"
#include "ports.h"
#include "stm8s_tim5.h"

static void up_0()
{
    TIM1->CCER1 = TIM1->CCER1 & (uint8_t)(~TIM1_CCER1_CC2NE) | TIM1_CCER1_CC1NE;
    TIM1->CCER2 = TIM1->CCER2 & (uint8_t)(~TIM1_CCER2_CC3NE) | TIM1_CCER2_CC4E;
}

static void up_1()
{
    TIM1->CCER1 = TIM1->CCER1 & (uint8_t)(~TIM1_CCER1_CC2NE) | TIM1_CCER1_CC1NE;
    TIM1->CCER2 = TIM1->CCER2 & (uint8_t)(~TIM1_CCER2_CC4E) | TIM1_CCER2_CC3NE;
}

static void up_2()
{
    TIM1->CCER1 = TIM1->CCER1 & (uint8_t)(~TIM1_CCER1_CC1NE) | TIM1_CCER1_CC2NE;
    TIM1->CCER2 = TIM1->CCER2 & (uint8_t)(~TIM1_CCER2_CC4E) | TIM1_CCER2_CC3NE;
}

static void up_3()
{
    TIM1->CCER1 = TIM1->CCER1 & (uint8_t)(~TIM1_CCER1_CC1NE) | TIM1_CCER1_CC2NE;
    TIM1->CCER2 = TIM1->CCER2 & (uint8_t)(~TIM1_CCER2_CC3NE) | TIM1_CCER2_CC4E;
}

static volatile uint8_t _stepperMode;
static volatile uint8_t _dimmed = TRUE;

void stepper_tim5_tick()
{
    _dimmed = TRUE;
    TIM5_Cmd(DISABLE);
}

void unset_dimmed()
{
    _dimmed = FALSE;
    TIM5_SetCounter(0);
    TIM5_ClearITPendingBit(TIM5_IT_UPDATE);
    TIM5_Cmd(ENABLE);
}

void stepper_next()
{
    if (GPIO_ReadInputPin(DIR_PORT, (GPIO_Pin_TypeDef)DIR_PIN) == RESET)
    {
        uint8_t stepperMode = --_stepperMode;
        if (stepperMode == 255)
        {
            _stepperMode = 3;
        }
    }
    else
    {
        uint8_t stepperMode = ++_stepperMode;
        if (stepperMode == 4)
        {
            _stepperMode = 0;
        }
    }

    switch (_stepperMode)
    {
    case 0:
        up_0();
        break;

    case 1:
        up_1();
        break;

    case 2:
        up_2();
        break;

    case 3:
        up_3();
        break;
    }

    unset_dimmed();
}

void stepper_resetAll()
{
    GPIO_WriteHigh(OUT_A_M_PORT, (GPIO_Pin_TypeDef)OUT_A_M_PIN);
    GPIO_WriteHigh(OUT_A_P_PORT, (GPIO_Pin_TypeDef)OUT_A_P_PIN);
    GPIO_WriteHigh(OUT_B_M_PORT, (GPIO_Pin_TypeDef)OUT_B_M_PIN);
    GPIO_WriteHigh(OUT_B_P_PORT, (GPIO_Pin_TypeDef)OUT_B_P_PIN);
}

void stepper_init()
{
    _stepperMode = 0;
    up_0();
}

void stepper_setThreshold(uint8_t value)
{
    if (_dimmed)
    {
        value >>= 1;
    }

    TIM1_SetCompare1(value);
    TIM1_SetCompare2(value);
    TIM1_SetCompare3(value);
    TIM1_SetCompare4(value);
}

void stepper_setEnabled(bool value)
{
    TIM1_CtrlPWMOutputs(value ? ENABLE : DISABLE);
}