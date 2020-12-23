/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32l0xx_hal.h"

#include "nfc04a1_conf.h"
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOA
#define ST25_GPO_Pin GPIO_PIN_3
#define ST25_GPO_GPIO_Port GPIOA
#define ST25_GPO_EXTI_IRQn EXTI2_3_IRQn
#define ST25_LPD_Pin GPIO_PIN_4
#define ST25_LPD_GPIO_Port GPIOA
#define INK_MOSI_Pin GPIO_PIN_5
#define INK_MOSI_GPIO_Port GPIOA
#define INK_CLK_Pin GPIO_PIN_6
#define INK_CLK_GPIO_Port GPIOA
#define INK_CS_Pin GPIO_PIN_7
#define INK_CS_GPIO_Port GPIOA
#define INK_DC_Pin GPIO_PIN_0
#define INK_DC_GPIO_Port GPIOB
#define INK_RST_Pin GPIO_PIN_1
#define INK_RST_GPIO_Port GPIOB
#define INK_IS_BUSY_Pin GPIO_PIN_8
#define INK_IS_BUSY_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
