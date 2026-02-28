#include "main.h"
#include "stm32f0xx_hal.h"
#include <stm32f0xx_hal_conf.h>
#include "hal_gpio.h"
#include "hal_gpio.c"
#include <stdio.h>
#include <stdint.h>
#include "stm32f0xx_it.h"

void SystemClock_Config(void);
volatile uint32_t WHO_AM_I = 0;
volatile uint32_t i2c_read_data = 0;

volatile uint8_t x_axis_data_low = 0;
volatile uint8_t x_axis_data_high = 0;
volatile uint16_t x_axis_data = 0;

volatile uint8_t y_axis_data_low = 0;
volatile uint8_t y_axis_data_high = 0;
volatile uint16_t y_axis_data = 0;



/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  HAL_RCC_GPIO_CLK_ENABLE(); 
  Init_GPIO(); 
  Init_I2C();
  //Setup_I2C_Transaction();
  I2C_Reg_Write(0x20, 0x0B);

  for (volatile int i = 0; i<300000; i++);
  while (1)
  {
    I2C_X_Y_Axis_Read();
    Determine_Active_LED();
    HAL_Delay(100);
    
    if (x_axis_data == 99999) { 
        I2C2->CR1 = 0;
    }
    if (y_axis_data == 99999) { 
        I2C2->CR1 = 0;}
    //My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    
  }
  return -1;
}

void Init_GPIO()
{ 
  GPIO_InitTypeDef initPB11 = {GPIO_PIN_11,
                              0x02,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW}; 
  GPIO_InitTypeDef initPB13 = {GPIO_PIN_13,
                              0x02,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};                                
  GPIO_InitTypeDef initPB14 = {GPIO_PIN_14,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};    
  GPIO_InitTypeDef initPB15 = {GPIO_PIN_15,
                              GPIO_MODE_INPUT,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};                                                          
  GPIO_InitTypeDef initPC0 = {GPIO_PIN_0,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};    
  GPIOB->OTYPER |= GPIO_OTYPER_OT_11;
  GPIOB->OTYPER |= GPIO_OTYPER_OT_13;

  GPIOB->AFR[1] &= ~(0xF << (3 * 4)); 
  GPIOB->AFR[1] &= ~(0xF << (5 * 4)); 

  GPIOB->AFR[1] |=  (0x1 << (3 * 4)); // PB11 to AF1
  GPIOB->AFR[1] |=  (0x5 << (5 * 4)); // PB13 to AF5                             

  My_HAL_GPIO_Init(GPIOB, &initPB11); 
  My_HAL_GPIO_Init(GPIOB, &initPB13);       
  My_HAL_GPIO_Init(GPIOB, &initPB14);
  My_HAL_GPIO_Init(GPIOB, &initPB15);
  My_HAL_GPIO_Init(GPIOC, &initPC0);  

  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
  My_HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);


  // LEDs
  GPIO_InitTypeDef initPC6 = {GPIO_PIN_6,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};
  GPIO_InitTypeDef initPC7 = {GPIO_PIN_7,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};
  GPIO_InitTypeDef initPC8 = {GPIO_PIN_8,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};                                                              
  GPIO_InitTypeDef initPC9 = {GPIO_PIN_9,
                              GPIO_MODE_OUTPUT_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};   
  My_HAL_GPIO_Init(GPIOC, &initPC6);                                                      
  My_HAL_GPIO_Init(GPIOC, &initPC7);                                                      
  My_HAL_GPIO_Init(GPIOC, &initPC8);                                                      
  My_HAL_GPIO_Init(GPIOC, &initPC9);                                                      
}


void Init_I2C()
{
  RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

  I2C2->CR1 &= ~I2C_CR1_PE;

  RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST;
  RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C2RST;

  uint32_t PRESC = 1;
  uint32_t SCLDEL = 4;
  uint32_t SDADEL = 2;
  uint32_t SCLH = 0x0F;
  uint32_t SCLL = 0x13;

  I2C2->TIMINGR = (PRESC << 28) | (SCLDEL << 20) | (SDADEL << 16) | (SCLH << 8) | (SCLL << 0);

  I2C2->CR1 |= I2C_CR1_PE;
}

void Setup_I2C_Transaction()
{ 
  I2C2->CR2 &= ~(I2C_CR2_SADD);
  I2C2->CR2 &= ~(I2C_CR2_NBYTES);
  I2C2->CR2 &= ~(I2C_CR2_RD_WRN); // Configure write operation
  I2C2->CR2 &= ~((0xFF << 16) | (0x3FF << 0));
  I2C2->CR2 |= (0x69 << 1); // Set slave address
  I2C2->CR2 |= (1 << 16); // Set number of data bytes
  
  I2C2->CR2 |= I2C_CR2_START; // Set start bit

  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_TXIS))){}

  if (I2C2->ISR & I2C_ISR_NACKF) { return; }

  I2C2->TXDR = 0x0F; // Write WHO_AM_I address

  while (!(I2C2->ISR & I2C_ISR_TC)) { } // Wait until TC flag is set

  I2C2->CR2 &= ~(I2C_CR2_SADD); 
  I2C2->CR2 &= ~(I2C_CR2_NBYTES);
  I2C2->CR2 &= ~(I2C_CR2_RD_WRN);

  I2C2->CR2 |= (0x69 << 1); 
  I2C2->CR2 |= (1 << 16); 
  I2C2->CR2 |= I2C_CR2_RD_WRN; // Configure read operation
  I2C2->CR2 |= I2C_CR2_START;

  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_RXNE))) {}

  if (I2C2->ISR & I2C_ISR_NACKF) { return; }

  WHO_AM_I = I2C2->RXDR;
  
  I2C2->CR1 |= I2C_CR1_PE;
  I2C2->CR2 |= I2C_CR2_STOP;
}

void I2C_Reg_Write(uint8_t reg_addr, uint8_t data)
{
  I2C2->CR2 &= ~(I2C_CR2_SADD);
  I2C2->CR2 &= ~(I2C_CR2_NBYTES);
  I2C2->CR2 &= ~(I2C_CR2_RD_WRN); // Configure write operation
  I2C2->CR2 &= ~((0xFF << 16) | (0x3FF << 0));
  I2C2->CR2 |= (0x69 << 1); // Set slave address
  I2C2->CR2 |= (2 << 16); // Set number of data bytes
  I2C2->CR2 |= I2C_CR2_START;

  // Byte 1 (Address)
  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_TXIS))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  I2C2->TXDR = reg_addr; // Write CTROL_REG1 address

  // Byte 2 (Data)
  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_TXIS))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  I2C2->TXDR = data; // Write CTROL_REG1 address

  while (!(I2C2->ISR & I2C_ISR_TC)) { } // Wait until TC flag is set
  I2C2->CR2 |= I2C_CR2_STOP;
}

void I2C_Reg_Read(uint8_t reg_addr, uint8_t data)
{
  // Write
  I2C2->CR2 &= ~(I2C_CR2_SADD);
  I2C2->CR2 &= ~(I2C_CR2_NBYTES);
  I2C2->CR2 &= ~(I2C_CR2_RD_WRN); // Configure write operation
  I2C2->CR2 &= ~((0xFF << 16) | (0x3FF << 0));
  I2C2->CR2 |= (0x69 << 1); // Set slave address
  I2C2->CR2 |= (1<< 16); // Set number of data bytes
  I2C2->CR2 |= I2C_CR2_START;

  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_TXIS))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  I2C2->TXDR = reg_addr; // Write CTRL_REG1 address

  while (!(I2C2->ISR & I2C_ISR_TC)) { } // Wait until TC flag is set

  // Read
  I2C2->CR2 &= ~(I2C_CR2_SADD);
  I2C2->CR2 &= ~(I2C_CR2_NBYTES);
  I2C2->CR2 &= ~(I2C_CR2_RD_WRN);
  I2C2->CR2 &= ~((0xFF << 16) | (0x3FF << 0));
  I2C2->CR2 |= (0x69 << 1); // Set slave address
  I2C2->CR2 |= (2 << 16); // Set number of data bytes
  I2C2->CR2 |= I2C_CR2_RD_WRN; // Configure read operation
  I2C2->CR2 |= I2C_CR2_START;

  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_RXNE))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  uint8_t i2c_read_data_low = I2C2->RXDR; 

  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_RXNE))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  uint8_t i2c_read_data_high = I2C2->RXDR; 

  i2c_read_data = (i2c_read_data_high << 8) | i2c_read_data_low;

  while (!(I2C2->ISR & I2C_ISR_TC)) {}
  I2C2->CR2 |= I2C_CR2_STOP;
}

void I2C_X_Y_Axis_Read()
{
  // Write
  I2C2->CR2 &= ~(I2C_CR2_SADD);
  I2C2->CR2 &= ~(I2C_CR2_NBYTES);
  I2C2->CR2 &= ~(I2C_CR2_RD_WRN); // Configure write operation
  I2C2->CR2 &= ~((0xFF << 16) | (0x3FF << 0));
  I2C2->CR2 |= (0x69 << 1); // Set slave address
  I2C2->CR2 |= (1 << 16); // Set number of data bytes
  //I2C2->CR2 |= I2C_CR2_START;

  I2C2->CR2 = 0; 
  I2C2->CR2 |= (0x69 << 1) | (1 << 16); // 1 Byte, Write Mode
  I2C2->CR2 |= I2C_CR2_START;

  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_TXIS))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  I2C2->TXDR = 0xA8; // X-Axis Address

  while (!(I2C2->ISR & I2C_ISR_TC)) {} // Wait until TC flag is set

  // Read
  I2C2->CR2 &= ~(I2C_CR2_SADD);
  I2C2->CR2 &= ~(I2C_CR2_NBYTES);
  I2C2->CR2 &= ~(I2C_CR2_RD_WRN);
  I2C2->CR2 &= ~((0xFF << 16) | (0x3FF << 0));
  I2C2->CR2 |= (0x69 << 1); // Set slave address
  I2C2->CR2 |= (4 << 16); // Read 4 data bytes
  I2C2->CR2 |= I2C_CR2_RD_WRN; // Configure read operation
  I2C2->CR2 |= I2C_CR2_START;

  // X-Axis
  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_RXNE))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  x_axis_data_low = I2C2->RXDR; 

  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_RXNE))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  x_axis_data_high = I2C2->RXDR; 

  // Y-Axis
  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_RXNE))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  y_axis_data_low = I2C2->RXDR; 

  while (!(I2C2->ISR & (I2C_ISR_NACKF | I2C_ISR_RXNE))){}
  if (I2C2->ISR & I2C_ISR_NACKF) { return; }
  y_axis_data_high = I2C2->RXDR; 


  while (!(I2C2->ISR & I2C_ISR_TC)) {}
  I2C2->CR2 |= I2C_CR2_STOP;

}

void Determine_Active_LED()
{
  // Combine bytes
  int16_t x_axis_data = (int16_t)( ((uint16_t)x_axis_data_high << 8) | (uint16_t)x_axis_data_low );
  int16_t y_axis_data = (int16_t)( ((uint16_t)y_axis_data_high << 8) | (uint16_t)y_axis_data_low );

  uint16_t threshold = 10000;

  if ((x_axis_data > threshold))
  {
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
  }
  else  if (x_axis_data < -threshold)
  {
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
  }
  else 
  {
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
  }

  if (y_axis_data > threshold){
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
  }
  else if (y_axis_data < -threshold){
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
  }
  else 
  {
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
  }
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
