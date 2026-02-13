#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

void Error_Handler(void);
void alternateLED(void);
void HAL_RCC_GPIOC_CLK_ENABLE(void);
void HAL_RCC_GPIOA_CLK_ENABLE(void);
void HAL_RCC_SYSCFG_CLK_ENABLE(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
