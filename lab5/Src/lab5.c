#include "main.h"
#include "stm32f0xx_hal.h"
#include <stm32f0xx_hal_conf.h>
#include "hal_gpio.h"
#include "hal_gpio.c"
#include <stdio.h>
#include "stm32f0xx_it.h"

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  HAL_RCC_GPIO_CLK_ENABLE(); 
  Init_I2C();
  Setup_I2C_Transaction();

  GPIO_InitTypeDef initPB6 = {GPIO_PIN_6,
                              GPIO_MODE_AF_OD, // Alternate Open-Drain 
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};
  GPIO_InitTypeDef initPB7 = {GPIO_PIN_7,
                              GPIO_MODE_AF_OD, // Alternate Open-Drain 
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};                            
 GPIO_InitTypeDef initPC9 = {GPIO_PIN_9,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};
  My_HAL_GPIO_Init(GPIOB, &initPB6);
  My_HAL_GPIO_Init(GPIOB, &initPB7);      
  My_HAL_GPIO_Init(GPIOC, &initPC9); 
  My_HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
  My_HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);     
  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);                          

  while (1)
  {
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    HAL_Delay(600);
 
  }
  return -1;
}

void Init_I2C()
{
  GPIOB->AFR[0] &= (0xF << ( 6 * 4 )); // PB6
  GPIOB->AFR[0] &= (0xF << ( 7 * 4 )); // PB7

  GPIOB->AFR[0] |= (0x1 << ( 6 * 4 )); // PB6
  GPIOB->AFR[0] |= (0x1 << ( 7 * 4 )); // PB7

  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  I2C1->CR1 &= ~I2C_CR1_PE;

  uint32_t PRESC = 0x10000000;
  uint32_t SCLDEL = 0x00400000;
  uint32_t SDADEL = 0x00020000;
  uint32_t SCLH = 0x00000F00;
  uint32_t SCLL = 0x00000013;

  I2C1->TIMINGR = PRESC + SCLDEL + SDADEL + SCLH + SCLL;

  I2C1->CR1 |= I2C_CR1_PE;
}

void Setup_I2C_Transaction()
{
  I2C1->CR2 &= ~(I2C_CR2_SADD); // Set slave address
  I2C1->CR2 |= (0x69 << 1);

  I2C1->CR2 &= ~(I2C_CR2_NBYTES); // Set number of data bytes
  I2C1->CR2 |= (1 << 16);

  I2C1->CR2 &= ~(I2C_CR2_RD_WRN); // Configure write operation

  I2C1->CR2 &= I2C_CR2_AUTOEND; // Do not set AUTOEND bit

  I2C1->CR2 |= I2C_CR2_START; // Set start bit
}




/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add their own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add their own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
