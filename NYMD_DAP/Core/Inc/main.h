/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TARGET_nRESET_Pin GPIO_PIN_9
#define TARGET_nRESET_GPIO_Port GPIOC
#define TARGET_NC_TDI_Pin GPIO_PIN_8
#define TARGET_NC_TDI_GPIO_Port GPIOA
#define TARGET_SWO_TDO_Pin GPIO_PIN_10
#define TARGET_SWO_TDO_GPIO_Port GPIOA
#define TARGET_SWCLK_TCK_Pin GPIO_PIN_11
#define TARGET_SWCLK_TCK_GPIO_Port GPIOA
#define TARGET_SWDIO_TMS_Pin GPIO_PIN_12
#define TARGET_SWDIO_TMS_GPIO_Port GPIOA
#define SB2_INPUT_Pin GPIO_PIN_7
#define SB2_INPUT_GPIO_Port GPIOB
#define LED_CONNECTED_Pin GPIO_PIN_8
#define LED_CONNECTED_GPIO_Port GPIOB
#define LED_RUNNING_Pin GPIO_PIN_9
#define LED_RUNNING_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
