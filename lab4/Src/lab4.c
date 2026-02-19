#include "main.h"
#include "stm32f0xx_hal.h"
#include <stm32f0xx_hal_conf.h>
#include "hal_gpio.h"
#include "hal_gpio.c"
#include <stdio.h>
#include "stm32f0xx_it.h"

void SystemClock_Config(void);

volatile char tempChar;
volatile char firstChar;
volatile char secondChar;
volatile uint16_t charCount;
volatile uint16_t promptCount;
volatile uint16_t newDataFlag = 0;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  HAL_RCC_GPIO_CLK_ENABLE(); 
  configure_GPIO_AFR();
  initialize_USART();
  void USART3_4_IRQHandler(void);

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
  GPIO_InitTypeDef initPB10 = {GPIO_PIN_10,
                              GPIO_MODE_AF_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};        
  GPIO_InitTypeDef initPB11 = {GPIO_PIN_11,
                              GPIO_MODE_AF_PP,
                              GPIO_NOPULL,
                              GPIO_SPEED_FREQ_LOW};    

  My_HAL_GPIO_Init(GPIOC, &initPC6);
  My_HAL_GPIO_Init(GPIOC, &initPC7);
  My_HAL_GPIO_Init(GPIOC, &initPC8);
  My_HAL_GPIO_Init(GPIOC, &initPC9);
  My_HAL_GPIO_Init(GPIOB, &initPB10);
  My_HAL_GPIO_Init(GPIOB, &initPB11);

  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
  My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

  while (1)
  {
    // Checkoff 1
    //toggle_LED(); 


    // Checkoff 2
    
    if ((promptCount == 0) && newDataFlag != 1)
    {
      transmit_string("Select LED:\n");
      promptCount = 3;
    }
    else if (promptCount == 1)
    {
       transmit_string("Select Action:\n");
       promptCount = 2;
    }

    if (newDataFlag)
    {
      newDataFlag = 0;
      command_parser();
    }
    
  }
  return -1;
}

// ================= Required Functions =================


void configure_GPIO_AFR(void)
{
  GPIOB->AFR[1] &= (0xF << ( 2 * 4 )); // PB10
  GPIOB->AFR[1] &= (0xF << ( 3 * 4 )); // PB11

  GPIOB->AFR[1] |= (0x4 << ( 2 * 4 )); // PB10
  GPIOB->AFR[1] |= (0x4 << ( 3 * 4 )); // PB11
}

void transmit_char(char character)
{
  while (!(USART3->ISR & USART_ISR_TXE)) {}
  USART3->TDR = character;
}

void transmit_string(char* string)
{
  while (*string != 0)
  {
    transmit_char(*string++);
  } 
  return;
}

// ==============================================


// ================= Checkoff 1 =================
/*
void initialize_USART(void)
{
  RCC->APB1ENR &= 0x12;
  RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

  uint32_t freq = HAL_RCC_GetHCLKFreq();
  uint32_t baud = 115200;
  USART3->BRR = freq / baud;

  USART3->CR1 |= USART_CR1_TE; // Transmitter Enable
  USART3->CR1 |= USART_CR1_RE; // Receiver Enable
  USART3->CR1 |= USART_CR1_UE; // USART Enable
}


void toggle_LED(void)
{
  uint16_t data;
  while (!(USART3->ISR & USART_ISR_RXNE)) {} 
  data = USART3->RDR;

  switch(data) {
  case 'r':
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
    break;
  case 'b':
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
    break;
  case 'o':
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    break;
  case 'g':
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    break;
  default:
    transmit_string("Error! No LED exists of that color");
    break;
  }
}
*/
// ==============================================


// ================= Checkoff 2 =================


void initialize_USART(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

  uint32_t freq = HAL_RCC_GetHCLKFreq();
  uint32_t baud = 115200;
  USART3->BRR = freq / baud;

  USART3->CR1 |= USART_CR1_TE; // Transmitter Enable
  USART3->CR1 |= USART_CR1_RE; // Receiver Enable
  USART3->CR1 |= USART_CR1_RXNEIE; // Receive Register Not Empty Interrupt Enable
  USART3->CR1 |= USART_CR1_UE; // USART Enable

  __NVIC_EnableIRQ(USART3_4_IRQn);
  __NVIC_SetPriority(USART3_4_IRQn, 3);
}



void USART3_4_IRQHandler(void) 
{
  if (USART3->ISR & USART_ISR_RXNE)
  {
    tempChar = (char)USART3->RDR;
    if (tempChar < 32) return;
    if (charCount == 0)
    {
      firstChar = tempChar;
      if (((firstChar != 'r') && (firstChar != 'g') && (firstChar != 'b') &&(firstChar != 'o')))
      {
        charCount = 0;
        promptCount = 0;
        transmit_string("Error! No LED exists of that color\n");
        return;
      }
      charCount = 1;
      promptCount = 1;
      
    }
    else if (charCount == 1)
    {
      secondChar = tempChar;
      if ((secondChar != '0') && (secondChar != '1') && (secondChar != '2'))
      {
        charCount = 0;
        promptCount = 0;
        transmit_string("Error! That is not a valid action\n");
      }
      charCount = 0;
      newDataFlag = 1;
      promptCount = 0;
    }
  }
}


void command_parser(void)
{
  //transmit_string("CMD?\n");
  switch(firstChar) {
  case 'r':
    switch(secondChar) {
      case '0':
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
        break;
      case '1':
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        break;
      case '2':
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        break;
      default:
        break;
    }
    break;
  case 'b':
    switch(secondChar) {
      case '0':
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        break;
      case '1':
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        break;
      case '2':
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        break;
      default:
        break;
    }
    break;
  case 'o':
   switch(secondChar) {
      case '0':
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        break;
      case '1':
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
        break;
      case '2':
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
        break;
      default:
        break;
    }
    break;
  case 'g':
    switch(secondChar) {
      case '0':
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
        break;
      case '1':
        My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
        break;
      case '2':
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
        break;
      default:
        break;
    }
    break;
  default:
    break;
  }
}



// ======================================================









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
