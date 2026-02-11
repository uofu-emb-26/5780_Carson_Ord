#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    uint32_t pos;
    for (pos = 0; pos < 16; pos++) {
        if ((GPIO_Init->Pin) & (1 << pos)) {
            GPIOx->MODER &= ~(0x3 << (pos * 2));
            GPIOx->MODER |= (GPIO_Init->Mode << (pos * 2));
            GPIOx->OSPEEDR &= ~(0x3 << (pos * 2));
            GPIOx->OSPEEDR |= (GPIO_Init->Speed << (pos * 2));
            GPIOx->PUPDR &= ~(0x3 << (pos * 2));
            GPIOx->PUPDR |= (GPIO_Init->Pull << (pos * 2));
        }
    }
}

GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{   
    return (GPIOx->IDR & GPIO_Pin);
}

void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    if (PinState == GPIO_PIN_SET) 
    {
        GPIOx->ODR |= GPIO_Pin;
    } else 
    {
        GPIOx->ODR &= ~GPIO_Pin;
    }
}

void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->ODR ^= GPIO_Pin;
}

void Button_To_EXTI(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    EXTI->IMR |= 0x1;
    EXTI->RTSR |= 0x1;
    EXTI->FTSR &= 0x0;
    
    SYSCFG->EXTICR[0] &= ~(0xF);
    SYSCFG->EXTICR[0] |= 0x0;
}

