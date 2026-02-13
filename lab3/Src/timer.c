#include "timer.h"
#include "stm32f0xx_hal.h"
#include "hal_gpio.h"
#include "stm32f072xb.h"
#include <stdio.h>


void timer2Setup() 
{
  RCC->APB1ENR |= 0x1;
  TIM2->PSC = 0x1F3F; // 7999
  TIM2->ARR = 0x7D; // 250
  TIM2->DIER |= 0x1;
  TIM2->CR1 |= TIM_CR1_CEN;
  __NVIC_EnableIRQ(TIM2_IRQn);
  __NVIC_SetPriority(TIM2_IRQn, 2);
}


void timer3Setup()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 30; //30
    TIM3->ARR = 125; //125
    TIM3->CCMR1 &= ~(0x70); //CC1S
    TIM3->CCMR1 |= (0x6 << 4); //CC1S
    TIM3->CCMR1 &= ~(0x180); //CC2S
    TIM3->CCMR1 |= (0x6 << 12); //CC1S
    TIM3->CCMR1 |= 0x70; //OCM1 set to PWM Mode 2
    TIM3->CCMR1 |= 0x6000; //OCM2 set to PWM Mode 1
    TIM3->CCMR1 |= 0x8; // Preload enable 1
    TIM3->CCMR1 |= 0x800; //Preload enable 2
    TIM3->CCER |= 0x1; // Output enable bit channel 1
    TIM3->CCER |= 0x10; // Output enable bit channel 2
    TIM3->CCR1 = 126; // Higher = dimmer
    TIM3->CCR2 = 126; // Higher = brighter
    TIM3->CR1 |= TIM_CR1_CEN;
}

void alternateLED(void)
{
  GPIOC->MODER &= ~(0x3 << (6 * 2));
  GPIOC->MODER |= (0x2 << (6 * 2));
  GPIOC->AFR[0] &= ~(0xF << (6 * 4));;

  GPIOC->MODER &= ~(0x3 << (7 * 2));
  GPIOC->MODER |= (0x2 << (7 * 2));
  GPIOC->AFR[0] &= ~(0xF << (7 * 4));;
}

