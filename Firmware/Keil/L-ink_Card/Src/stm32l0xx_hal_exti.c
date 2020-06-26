/**
  ******************************************************************************
  * @file    stm32l0xx_hal_exti.c
  * @author  MCD Application Team
  * @brief   EXTI HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the General Purpose Input/Output (EXTI) peripheral:
  *           + Initialization and de-initialization functions
  *           + IO operation functions
  *
  @verbatim
  ==============================================================================
                    ##### EXTI Peripheral features #####
  ==============================================================================
  [..]
    (+) Each Exti line can be configured within this driver.

    (+) Exti line can be configured in 3 different modes
        (++) Interrupt
        (++) Event
        (++) Both of them

    (+) Configurable Exti lines can be configured with 3 different triggers
        (++) Rising
        (++) Falling
        (++) Both of them

    (+) When set in interrupt mode, configurable Exti lines have two diffenrents
        interrupt pending registers which allow to distinguish which transition 
        occurs:
        (++) Rising edge pending interrupt
        (++) Falling 

    (+) Exti lines 0 to 15 are linked to gpio pin number 0 to 15. Gpio port can
        be selected throught multiplexer.

                     ##### How to use this driver #####
  ==============================================================================
  [..]

    (#) Configure the EXTI line using HAL_EXTI_SetConfigLine().
        (++) Choose the interrupt line number by setting "Line" member from
             EXTI_ConfigTypeDef structure.
        (++) Configure the interrupt and/or event mode using "Mode" member from
             EXTI_ConfigTypeDef structure.
        (++) For configurable lines, configure rising and/or falling trigger
             "Trigger" member from EXTI_ConfigTypeDef structure.
        (++) For Exti lines linked to gpio, choose gpio port using "GPIOSel"
             member from GPIO_InitTypeDef structure.

    (#) Get current Exti configuration of a dedicated line using 
        HAL_EXTI_GetConfigLine().
        (++) Provide exiting handle as parameter.
        (++) Provide pointer on EXTI_ConfigTypeDef structure as second parameter.

    (#) Clear Exti configuration of a dedicated line using HAL_EXTI_GetConfigLine().
        (++) Provide exiting handle as parameter.

    (#) Register callback to treat Exti interrupts using HAL_EXTI_RegisterCallback().
        (++) Provide exiting handle as first parameter.
        (++) Provide which callback will be registered using one value from
             EXTI_CallbackIDTypeDef.
        (++) Provide callback function pointer.

    (#) Get interrupt pending bit using HAL_EXTI_GetPending().

    (#) Clear interrupt pending bit using HAL_EXTI_GetPending().

    (#) Generate software interrupt using HAL_EXTI_GenerateSWI().

  @endverbatim
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal_exti.h"

/** @addtogroup STM32L0xx_HAL_Driver
  * @{
  */

/** @addtogroup EXTI
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private defines ------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup EXTI_Exported_Functions
  * @{
  */

/** @addtogroup EXTI_Exported_Functions_Group1
 *  @brief    Configuration functions
 *
@verbatim
 ===============================================================================
              ##### Configuration functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Register callback for a dedicaated Exti line.
  * @param  hexti Exti handle.
  * @param  CallbackID User callback identifier.
  *         This parameter can be one of @arg @ref EXTI_CallbackIDTypeDef values.
  * @param  pPendingCbfn function pointer to be stored as callback.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void))
{
  HAL_StatusTypeDef status = HAL_OK;

  switch (CallbackID)
  {
#ifdef EXTI_IMR1_IM0
    case  HAL_EXTI_COMMON_CB_ID:
      hexti->RisingCallback = pPendingCbfn;
      hexti->FallingCallback = pPendingCbfn;
      break;

    case  HAL_EXTI_RISING_CB_ID:
      hexti->RisingCallback = pPendingCbfn;
      break;

    case  HAL_EXTI_FALLING_CB_ID:
      hexti->FallingCallback = pPendingCbfn;
      break; 
#else
    case  HAL_EXTI_COMMON_CB_ID:
      hexti->Callback = pPendingCbfn;
      break;      
#endif
    default:
      status = HAL_ERROR;
      break;
  }

  return status;
}

/**
  * @brief  Store line number as handle private field.
  * @param  hexti Exti handle.
  * @param  ExtiLine Exti line number.
  *         This parameter can be from 0 to @ref EXTI_LINE_NB.
  * @retval HAL Status.
  */
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine)
{
  /* Check the parameters */
  assert_param(IS_EXTI_LINE(ExtiLine));

  /* Check null pointer */
  if(hexti == NULL)
  {
    return HAL_ERROR;
  }
  else
  {
    /* Store line number as handle private field */
    hexti->Line = ExtiLine;
    return HAL_OK;
  }
}

/**
  * @}
  */

/** @addtogroup EXTI_Exported_Functions_Group2
 *  @brief EXTI IO functions.
 *
@verbatim
 ===============================================================================
                       ##### IO operation functions #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Handle EXTI interrupt request.
  * @param  hexti Exti handle.
  * @retval none.
  */
void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti)
{
  uint32_t maskline;
  uint32_t linepos;
#if defined(EXTI_D1)
  EXTI_Core_TypeDef *EXTI_CurrentCPU;
#if defined(DUAL_CORE) && defined(CORE_CM4)
  EXTI_CurrentCPU = EXTI_D2; /* EXTI for CM4 CPU */
#else
  EXTI_CurrentCPU = EXTI_D1; /* EXTI for CM7 CPU */
#endif
#endif /* EXTI_D1 */

  linepos = (hexti->Line & EXTI_PIN_MASK);
  maskline = (1 << linepos);
  
#if defined(EXTI_D1)

  if((EXTI_CurrentCPU->PR1 & maskline) != 0x00u)
  {
    /* Clear pending bit */
    EXTI_CurrentCPU->PR1 = maskline;
    
    /* Call rising/Falling callback */
    if(hexti->RisingCallback != NULL)
    {
      hexti->RisingCallback();
    }
    
    if(hexti->FallingCallback != NULL)
    {
      hexti->FallingCallback();
    }  
  }
#elif defined(EXTI_IMR1_IM0)
  if((EXTI->PR1 & maskline) != 0x00u)
  {
    /* Clear pending bit */
    EXTI->PR1 = maskline;
    
    /* Call rising/Falling callback */
    if(hexti->RisingCallback != NULL)
    {
      hexti->RisingCallback();
    }
    
    if(hexti->FallingCallback != NULL)
    {
      hexti->FallingCallback();
    }  
  }
#else  
  /* compute line mask */
  if((EXTI->PR & maskline) != 0x00u)
  {
    /* Clear pending bit */
    EXTI->PR = maskline;
    
    /* Call rising/Falling callback */
    if(hexti->Callback != NULL)
    {
      hexti->Callback();
    }  
  }
#endif
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
