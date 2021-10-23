/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm8s_it.h" /* SDCC patch: required by SDCC for interrupts */
#include "ports.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(uint32_t nCount);

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

void GPIO_setup()
{
  GPIO_Init(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LED_GPIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);

  GPIO_Init(OUT_A_M_PORT, (GPIO_Pin_TypeDef)OUT_A_M_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(OUT_A_P_PORT, (GPIO_Pin_TypeDef)OUT_A_P_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(OUT_B_M_PORT, (GPIO_Pin_TypeDef)OUT_B_M_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(OUT_B_P_PORT, (GPIO_Pin_TypeDef)OUT_B_P_PIN, GPIO_MODE_OUT_PP_LOW_FAST);

  GPIO_Init(PUL_PORT, (GPIO_Pin_TypeDef)PUL_PIN, GPIO_MODE_IN_FL_IT);
  GPIO_Init(DIR_PORT, (GPIO_Pin_TypeDef)DIR_PIN, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(ENA_PORT, (GPIO_Pin_TypeDef)ENA_PIN, GPIO_MODE_IN_FL_NO_IT);

  GPIO_Init(SW1_PORT, (GPIO_Pin_TypeDef)SW1_PIN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(SW2_PORT, (GPIO_Pin_TypeDef)SW2_PIN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(SW3_PORT, (GPIO_Pin_TypeDef)SW3_PIN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(SW4_PORT, (GPIO_Pin_TypeDef)SW4_PIN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(SW5_PORT, (GPIO_Pin_TypeDef)SW5_PIN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(SW6_PORT, (GPIO_Pin_TypeDef)SW6_PIN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(SW7_PORT, (GPIO_Pin_TypeDef)SW7_PIN, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(SW8_PORT, (GPIO_Pin_TypeDef)SW8_PIN, GPIO_MODE_IN_PU_NO_IT);

  GPIO_Init(CurrentA_PORT, (GPIO_Pin_TypeDef)CurrentA_PIN, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(CurrentB_PORT, (GPIO_Pin_TypeDef)CurrentB_PIN, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(PowerControl_PORT, (GPIO_Pin_TypeDef)PowerControl_PIN, GPIO_MODE_IN_FL_NO_IT);
}

void CLOCK_setup()
{
  CLK_DeInit();

  CLK_HSECmd(DISABLE);
  CLK_LSICmd(DISABLE);
  CLK_HSICmd(ENABLE);
  while (CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE)
    ;

  CLK_ClockSwitchCmd(ENABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
                        DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER5, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER6, DISABLE);
}

void TIM5_setup()
{
  TIM5_DeInit();
  TIM5_TimeBaseInit(TIM5_PRESCALER_512, 15625); // 500 ms
  TIM5_ARRPreloadConfig(ENABLE);                // Enable automatic reloading
  TIM5_ITConfig(TIM5_IT_UPDATE, ENABLE);
}

void EXTI_setup()
{
  //ITC_DeInit();
  //ITC_SetSoftwarePriority(ITC_IRQ_PORTC, ITC_PRIORITYLEVEL_0);

  EXTI_DeInit();
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_FALL_ONLY);
  EXTI_SetTLISensitivity(EXTI_TLISENSITIVITY_FALL_ONLY);
}

void ADC1_setChannel(ADC1_Channel_TypeDef ADC1_Channel)
{
  /* Clear the ADC1 channels */
  ADC1->CSR &= (uint8_t)(~ADC1_CSR_CH);
  /* Select the ADC1 channel */
  ADC1->CSR |= (uint8_t)(ADC1_Channel);
}

void ADC1_setup()
{
  ADC1_DeInit();

  /*-----------------CR1 & CSR configuration --------------------*/
  /* Configure the conversion mode and the channel to convert
  respectively according to ADC1_ConversionMode & ADC1_Channel values  &  ADC1_Align values */
  ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_0, ADC1_ALIGN_RIGHT);
  /* Select the prescaler division factor according to ADC1_PrescalerSelection values */
  ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D8);
  //ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
  //ADC1_ScanModeCmd(ENABLE);
  ADC1_Cmd(ENABLE);
}

uint16_t getAdcValue(ADC1_Channel_TypeDef channel)
{
  ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, channel, ADC1_ALIGN_RIGHT);
  ADC1_StartConversion();
  while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET)
    ;
  ADC1_ClearFlag(ADC1_FLAG_EOC);
  return ADC1_GetConversionValue();
}

bool checkPower()
{
  uint16_t a1 = getAdcValue(ADC1_CHANNEL_2);
  return (a1 >= (uint16_t)169); // 825 mV
}

uint8_t GetThresholdValue()
{
  uint8_t value = 0;

  if (GPIO_ReadInputPin(SW1_PORT, (GPIO_Pin_TypeDef)SW1_PIN) == RESET)
    value |= 1 << 0;
  if (GPIO_ReadInputPin(SW2_PORT, (GPIO_Pin_TypeDef)SW2_PIN) == RESET)
    value |= 1 << 1;
  if (GPIO_ReadInputPin(SW3_PORT, (GPIO_Pin_TypeDef)SW3_PIN) == RESET)
    value |= 1 << 2;
  if (GPIO_ReadInputPin(SW4_PORT, (GPIO_Pin_TypeDef)SW4_PIN) == RESET)
    value |= 1 << 3;
  if (GPIO_ReadInputPin(SW5_PORT, (GPIO_Pin_TypeDef)SW5_PIN) == RESET)
    value |= 1 << 4;
  if (GPIO_ReadInputPin(SW6_PORT, (GPIO_Pin_TypeDef)SW6_PIN) == RESET)
    value |= 1 << 5;
  if (GPIO_ReadInputPin(SW7_PORT, (GPIO_Pin_TypeDef)SW7_PIN) == RESET)
    value |= 1 << 6;
  if (GPIO_ReadInputPin(SW8_PORT, (GPIO_Pin_TypeDef)SW8_PIN) == RESET)
    value |= 1 << 7;

  return value;
}

void TIM1_setup()
{
  const uint16_t tim_prescaler = (uint16_t)255UL;

  TIM1_DeInit();
  TIM1_TimeBaseInit((uint16_t)2, TIM1_COUNTERMODE_UP, tim_prescaler, 0);

  // Channel B- PWM configuration
  TIM1_OC1Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_DISABLE, TIM1_OUTPUTNSTATE_ENABLE, 0,
               TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_SET, TIM1_OCNIDLESTATE_SET);
  TIM1_OC1PreloadConfig(ENABLE);

  // Channel B+ PWM configuration
  TIM1_OC2Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_DISABLE, TIM1_OUTPUTNSTATE_ENABLE, 0,
               TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_RESET);
  TIM1_OC2PreloadConfig(ENABLE);

  // Channel A- PWM configuration
  TIM1_OC3Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_DISABLE, TIM1_OUTPUTNSTATE_ENABLE, 0,
               TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_HIGH, TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_RESET);
  TIM1_OC3PreloadConfig(ENABLE);

  // Channel A+ PWM configuration
  TIM1_OC4Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, 0,
               TIM1_OCPOLARITY_HIGH, TIM1_OCIDLESTATE_RESET);
  TIM1_OC4PreloadConfig(ENABLE);

  TIM1_ARRPreloadConfig(ENABLE); // Enable automatic reloading

  TIM1_Cmd(ENABLE);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  CLOCK_setup();
  GPIO_setup();
  stepper_resetAll();
  EXTI_setup();
  TIM5_setup();
  TIM1_setup();
  ADC1_setup();

  /************/

  stepper_init();

  enableInterrupts();

  while (1)
  {
    if (checkPower())
    {
      GPIO_WriteHigh(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LED_GPIO_PIN);
    }
    else
    {
      GPIO_WriteLow(LED_GPIO_PORT, (GPIO_Pin_TypeDef)LED_GPIO_PIN);
    }

    stepper_setEnabled(GPIO_ReadInputPin(ENA_PORT, (GPIO_Pin_TypeDef)ENA_PIN) == RESET);

    uint8_t thresholdValue = GetThresholdValue();
    stepper_setThreshold(thresholdValue);
  }
}

/**
  * @brief Delay
  * @param nCount
  * @retval None
  */
void Delay(uint32_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif