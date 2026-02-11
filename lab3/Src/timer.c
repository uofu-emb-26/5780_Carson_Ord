#include "timer.h"
#include "stm32f0xx_hal.h"
#include "hal_gpio.h"


void timer2Setup(void) 
{
  RCC->APB1ENR |= 0x1;
  TIM2->PSC = 0x1F3F; // 7900
  TIM2->ARR = 0x7D; // 250
  TIM2->DIER |= 0x1;
  TIM2->CR1 |= 0x1;
  __NVIC_EnableIRQ(TIM2_IRQn);
  __NVIC_SetPriority(TIM2_IRQn, 2);
}


void timer3Setup(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 0x18F;
    TIM3->ARR = 0x2;
    TIM3->CCMR1 &= 0x0; // CC1S
    TIM3->CCMR1 &= 0x180; //CC2S
    TIM3->CCMR1 |= 0x70; //OCM1 set to PWM Mode 2
    TIM3->CCMR1 |= 0x6000; //OCM2 set to PWM Mode 1
    TIM3->CCMR1 |= 0x8; // Preload enable 1
    TIM3->CCMR1 |= 0x800; //Preload enable 2
    TIM3->CCER |= 0x1; // Output enable bit channel 1
    TIM3->CCER |= 0x10; // Output enable bit channel 2
}

