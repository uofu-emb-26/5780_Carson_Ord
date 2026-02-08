#include "main.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"
#include "hal_gpio.c"

volatile uint32_t count = 0;
volatile uint32_t handlerCount = 0;

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
   while (1)
  {
  }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  
  if (count == 200)
  {  
   My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
   count = 0;
  }
  count++;
  
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

void EXTI0_1_IRQHandler(void)
{
  EXTI->PR |= 0x1;
  My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
  My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
  
  for (handlerCount = 0; handlerCount < 1000000; handlerCount++) { }

  My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
  EXTI->PR |= 0x1;
} 
