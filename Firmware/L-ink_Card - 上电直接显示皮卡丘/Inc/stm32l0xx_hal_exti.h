/**
  ******************************************************************************
  * @file    stm32l0xx_hal_exti.h
  * @author  MCD Application Team
  * @brief   Header file of EXTI HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32L0xx_HAL_EXTI_H
#define STM32L0xx_HAL_EXTI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup EXTI EXTI
  * @brief EXTI HAL module driver
  * @{
  */
/* Exported types ------------------------------------------------------------*/

/** @defgroup EXTI_Exported_Types EXTI Exported Types
  * @{
  */
typedef enum
{
  HAL_EXTI_COMMON_CB_ID          = 0x00U,
#ifdef EXTI_IMR1_IM0
  HAL_EXTI_RISING_CB_ID          = 0x01U,
  HAL_EXTI_FALLING_CB_ID         = 0x02U,  
#endif
}EXTI_CallbackIDTypeDef;

/**
  * @brief  EXTI Handle structure definition
  */
typedef struct
{
  uint32_t Line;                    /*!<  Exti line number */
#ifdef EXTI_IMR1_IM0
  void (* RisingCallback) (void);   /*!<  Exti rising callback */
  void (* FallingCallback) (void);  /*!<  Exti falling callback */
#else  
  void (* Callback) (void);         /*!<  Exti common callback */
#endif  
}EXTI_HandleTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup EXTI_Exported_Constants EXTI Exported Constants
  * @{
  */
#define HAL_EXTI_DELIVERED 1

/** @defgroup EXTI_Line  EXTI Mode
  * @{
  */
#define EXTI_LINE_0                         (EXTI_GPIO   | 0x00u)
#define EXTI_LINE_1                         (EXTI_GPIO   | 0x01u)
#define EXTI_LINE_2                         (EXTI_GPIO   | 0x02u)
#define EXTI_LINE_3                         (EXTI_GPIO   | 0x03u)
#define EXTI_LINE_4                         (EXTI_GPIO   | 0x04u)
#define EXTI_LINE_5                         (EXTI_GPIO   | 0x05u)
#define EXTI_LINE_6                         (EXTI_GPIO   | 0x06u)
#define EXTI_LINE_7                         (EXTI_GPIO   | 0x07u)
#define EXTI_LINE_8                         (EXTI_GPIO   | 0x08u)
#define EXTI_LINE_9                         (EXTI_GPIO   | 0x09u)
#define EXTI_LINE_10                        (EXTI_GPIO   | 0x0Au)
#define EXTI_LINE_11                        (EXTI_GPIO   | 0x0Bu)
#define EXTI_LINE_12                        (EXTI_GPIO   | 0x0Cu)
#define EXTI_LINE_13                        (EXTI_GPIO   | 0x0Du)
#define EXTI_LINE_14                        (EXTI_GPIO   | 0x0Eu)
#define EXTI_LINE_15                        (EXTI_GPIO   | 0x0Fu)

/**
  * @}
  */

/** @defgroup EXTI_Mode  EXTI Mode
  * @{
  */
#define EXTI_MODE_NONE                      0x00000000u
#define EXTI_MODE_INTERRUPT                 0x00000001u
#define EXTI_MODE_EVENT                     0x00000002u
/**
  * @}
  */

/** @defgroup EXTI_Trigger  EXTI Trigger
  * @{
  */
#define EXTI_TRIGGER_NONE                   0x00000000u
#define EXTI_TRIGGER_RISING                 0x00000001u
#define EXTI_TRIGGER_FALLING                0x00000002u
#define EXTI_TRIGGER_RISING_FALLING         (EXTI_TRIGGER_RISING | EXTI_TRIGGER_FALLING)
/**
  * @}
  */

/** @defgroup EXTI_GPIOSel  EXTI GPIOSel
  * @brief
  * @{
  */
#define EXTI_GPIOA                          0x00000000u
#define EXTI_GPIOB                          0x00000001u
#define EXTI_GPIOC                          0x00000002u
#define EXTI_GPIOD                          0x00000003u
#define EXTI_GPIOE                          0x00000004u
#define EXTI_GPIOF                          0x00000005u
#define EXTI_GPIOG                          0x00000006u
#define EXTI_GPIOH                          0x00000007u
#define EXTI_GPIOI                          0x00000008u
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup EXTI_Exported_Macros EXTI Exported Macros
  * @{
  */

/**
  * @}
  */

/* Private constants --------------------------------------------------------*/
/** @defgroup EXTI_Private_Constants EXTI Private Constants
  * @{
  */
    
   
/**
  * @brief  EXTI Line property definition
  */
#define EXTI_PROPERTY_SHIFT                  24u
#define EXTI_DIRECT                         (0x01u << EXTI_PROPERTY_SHIFT)
#define EXTI_CONFIG                         (0x02u << EXTI_PROPERTY_SHIFT)
#define EXTI_GPIO                           ((0x04u << EXTI_PROPERTY_SHIFT) | EXTI_CONFIG)
#define EXTI_RESERVED                       (0x08u << EXTI_PROPERTY_SHIFT)
#define EXTI_PROPERTY_MASK                  (EXTI_DIRECT | EXTI_CONFIG | EXTI_GPIO)

    /**
  * @brief  EXTI Line number
  */
#define EXTI_LINE_NB                        16u
    
/**
  * @brief  EXTI Register and bit usage
  */
#define EXTI_PIN_MASK                       0x1Fu

/**
  * @brief  EXTI Mask for interrupt & event mode
  */
#define EXTI_MODE_MASK                      (EXTI_MODE_EVENT | EXTI_MODE_INTERRUPT)

/**
  * @brief  EXTI Mask for trigger possibilities
  */
#define EXTI_TRIGGER_MASK                   (EXTI_TRIGGER_RISING | EXTI_TRIGGER_FALLING)

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup EXTI_Private_Macros EXTI Private Macros
  * @{
  */
#define IS_EXTI_LINE(__LINE__)          (((((__LINE__) & EXTI_PROPERTY_MASK) == EXTI_DIRECT) || \
                                         (((__LINE__) & EXTI_PROPERTY_MASK) == EXTI_CONFIG)   || \
                                         (((__LINE__) & EXTI_PROPERTY_MASK) == EXTI_GPIO))    && \
                                         (((__LINE__) & EXTI_PIN_MASK) < EXTI_LINE_NB ))

#define IS_EXTI_GPIO_PORT(__PORT__)     (((__PORT__) == EXTI_GPIOA) || \
                                         ((__PORT__) == EXTI_GPIOB) || \
                                         ((__PORT__) == EXTI_GPIOC) || \
                                         ((__PORT__) == EXTI_GPIOD) || \
                                         ((__PORT__) == EXTI_GPIOE) || \
                                         ((__PORT__) == EXTI_GPIOF) || \
                                         ((__PORT__) == EXTI_GPIOG) || \
                                         ((__PORT__) == EXTI_GPIOH) || \
                                         ((__PORT__) == EXTI_GPIOI))

/**
  * @}
  */
  

/* Exported functions --------------------------------------------------------*/
/** @defgroup EXTI_Exported_Functions EXTI Exported Functions
 *  @brief    EXTI Exported Functions
  * @{
  */

/** @defgroup EXTI_Exported_Functions_Group1 Configuration functions
 *  @brief    Configuration functions
 * @{
 */
/* Configuration functions ****************************************************/
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void));
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine);
/**
  * @}
  */

/** @defgroup EXTI_Exported_Functions_Group2 IO operation functions
 *  @brief    IO operation functions
 * @{
 */
/* IO operation functions *****************************************************/
void              HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32L0_HAL_EXTI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
