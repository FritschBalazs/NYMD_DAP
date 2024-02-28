/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define JOY_BOUNCE_VAL 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
GPIO_TypeDef* joy_PORTS[] = {JOY_RIGHT_GPIO_Port, JOY_LEFT_GPIO_Port, JOY_UP_GPIO_Port, JOY_DOWN_GPIO_Port, JOY_SEL_GPIO_Port};
const uint16_t joy_PINS[] = {JOY_RIGHT_Pin, JOY_LEFT_Pin, JOY_UP_Pin, JOY_DOWN_Pin, JOY_SEL_Pin};

uint8_t joy_cnt[5];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  uint8_t data[] = "Hello world qwertzuiopasdfghjklyxcvbnm=+-*/123456789 ";
  uint8_t uart_sendonce_up = 0;
  uint8_t uart_sendonce_dwn = 0;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  //turn off LEDs
  HAL_GPIO_WritePin( LED1_GRE_GPIO_Port,  LED1_GRE_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin( LED2_ORG_GPIO_Port,  LED2_ORG_Pin, GPIO_PIN_RESET);   //offset LED2 for blinking effect
  HAL_GPIO_WritePin( LED3_RED_GPIO_Port,  LED3_RED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin( LED4_BLU_GPIO_Port,  LED4_BLU_Pin, GPIO_PIN_SET);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  /* read buttons, right,left,up,down,center*/
	  for (int i=0; i<5; i++){
		  if (HAL_GPIO_ReadPin(joy_PORTS[i], joy_PINS[i]) && joy_cnt[i] < JOY_BOUNCE_VAL){
			  joy_cnt[i]++;
		  }else if (joy_cnt[i] > 0){
			  joy_cnt[i]--;
		  }
	  }

	  //joy up
	  if (joy_cnt[2] == JOY_BOUNCE_VAL){
		  HAL_GPIO_WritePin(LED1_GRE_GPIO_Port, LED1_GRE_Pin, GPIO_PIN_RESET);
		  if (uart_sendonce_up == 0){
			  HAL_UART_Transmit_IT(&huart1, data, 12);
			  uart_sendonce_up = 1;
		  }

	  }else if (joy_cnt[2] == 0 ){
		  HAL_GPIO_WritePin(LED1_GRE_GPIO_Port, LED1_GRE_Pin, GPIO_PIN_SET);
		  uart_sendonce_up = 0;
	  }

	  //joy down
	  if (joy_cnt[3] == JOY_BOUNCE_VAL){
		  HAL_GPIO_WritePin(LED3_RED_GPIO_Port, LED3_RED_Pin, GPIO_PIN_RESET);
		  if (uart_sendonce_dwn == 0){
			  HAL_UART_Transmit_IT(&huart1, data, 4);
			  uart_sendonce_dwn = 1;
		  }

	  }else if (joy_cnt[3] == 0 ){
		  HAL_GPIO_WritePin(LED3_RED_GPIO_Port, LED3_RED_Pin, GPIO_PIN_SET);
		  uart_sendonce_dwn = 0;
	  }

	  if((HAL_GetTick() % 500) <= 10){
		  HAL_GPIO_TogglePin( LED4_BLU_GPIO_Port,  LED4_BLU_Pin);
		  HAL_GPIO_TogglePin( LED2_ORG_GPIO_Port,  LED2_ORG_Pin);
		  HAL_UART_Transmit( &huart2,  data,  5,  1000 );
	  }

	  HAL_Delay(10);






    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED3_RED_Pin|LED4_BLU_Pin|LED1_GRE_Pin|LED2_ORG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED3_RED_Pin LED4_BLU_Pin LED1_GRE_Pin LED2_ORG_Pin */
  GPIO_InitStruct.Pin = LED3_RED_Pin|LED4_BLU_Pin|LED1_GRE_Pin|LED2_ORG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : JOY_SEL_Pin */
  GPIO_InitStruct.Pin = JOY_SEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(JOY_SEL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : JOY_RIGHT_Pin JOY_LEFT_Pin */
  GPIO_InitStruct.Pin = JOY_RIGHT_Pin|JOY_LEFT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : JOY_UP_Pin JOY_DOWN_Pin */
  GPIO_InitStruct.Pin = JOY_UP_Pin|JOY_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void setup_TRACE(void){
  uint32_t cpuCoreFreqHz = 72000000; // 72 MHz
  uint32_t SWOSpeed = 115200; // 115200 baud rate
  uint32_t SWOPrescaler = (cpuCoreFreqHz / SWOSpeed) - 1;

  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // DWT and ITM global enable
  DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN; // STM32F429 MCU specific Trace enable

  DWT->FUNCTION0 = 0x0000002F;
  // Generate Data trace data value packet on write accesses
  DWT->COMP0 = (uint32_t)&joy_cnt[2]; // Holds the observed address

  TPI->ACPR = SWOPrescaler; // Setting the prescaler for the wished baudrate
  TPI->SPPR = 2;

  // Selected Pin Protocol register[1:0] = NRZ coding selected
  ITM->LAR = 0xC5ACCE55; // Lock Access Register, Value required to unlock : 0xC5ACCE55
  while (((ITM->TCR & 0x00800000) == 0x00800000) == 1) // Wait until the ITM becomes inactive
  {
    HAL_Delay(1000);
    // polling
  }
  /* Trace Control Register: TSPrescale bits[9:8] = SYS CLK division by 64 ;
    TXENA bit[3]: = 1 enable forwarding of DWT packets;
    TSENA bit[1] = 1 enable local timestamp generation; ITMENA bit[0] = 1 enable ITM */

  ITM->TCR = 0x0000030B;
  ITM->TER = 0xFFFFFFFF; // Trace Enable Register: All ports enabled
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
