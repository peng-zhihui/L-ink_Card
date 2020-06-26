/* ****************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 **************************************************************************** */

/* Define to prevent redundant inclusion */
#ifndef _MXC_RTC_REGS_H_
#define _MXC_RTC_REGS_H_

/* **** Includes **** */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
    If types are not defined elsewhere (CMSIS) define them here
*/
/// @cond
#ifndef __IO
#define __IO volatile
#endif
#ifndef __I
#define __I  volatile const
#endif
#ifndef __O
#define __O  volatile
#endif
#ifndef __R
#define __R  volatile const
#endif
/// @endcond

/**
 * @defgroup    rtc_registers RTC Registers
 * @brief       Registers, Bit Masks and Bit Positions
 * @ingroup     rtc
 * @{
 */

/**
 * Structure type for the Real-Time Clock module registers allowing direct 32-bit access to each register.
 */
 typedef struct {
    __IO uint32_t ctrl;                                 /**< <tt>\b 0x0000: </tt> RTC_CTRL Register - RTC Timer Control                                                          */
    __IO uint32_t timer;                                /**< <tt>\b 0x0004: </tt> RTC_TIMER Register - RTC Timer Count Value                                            */
    __IO uint32_t comp[2];                              /**< <tt>\b 0x0008-0x000C: </tt> RTC_COMP0/RTC_COMP1 Registers - RTC Time of Day Alarm [0..1] Compare Register */
    __IO uint32_t flags;                                /**< <tt>\b 0x0010: </tt> RTC_FLAGS Register - CPU Interrupt and RTC Domain Flags                               */
    __IO uint32_t snz_val;                              /**< <tt>\b 0x0014: </tt> RTC_SNZ_VAL Register - RTC Timer Alarm Snooze Value                                   */
    __IO uint32_t inten;                                /**< <tt>\b 0x0018: </tt> RTC_INTEN Register - Interrupt Enable Controls                                        */
    __IO uint32_t prescale;                             /**< <tt>\b 0x001C: </tt> RTC_PRESCALE Register - RTC Timer Prescale Setting                                    */
    __R  uint32_t rsv020;                               /**< <tt>\b 0x0020: </tt> RESERVED                                                                              */
    __IO uint32_t prescale_mask;                        /**< <tt>\b 0x0024: </tt> RTC_PRESCALE_MASK Register - RTC Timer Prescale Compare Mask                          */
    __IO uint32_t trim_ctrl;                            /**< <tt>\b 0x0028: </tt> RTC_TRIM_CTRL Register - RTC Timer Trim Controls                                      */
    __IO uint32_t trim_value;                           /**< <tt>\b 0x002C: </tt> RTC_TRIM_VALUE Register - RTC Timer Trim Adjustment Interval                          */
} mxc_rtctmr_regs_t;


/**
 * Structure type for access to the RTC CFG hardware.
 */
typedef struct {
    __IO uint32_t nano_cntr;                            /**< <tt>\b 0x0000: </tt> - RTCCFG_NANO_CNTR - Nano Oscillator Counter Read Register    */
    __IO uint32_t clk_ctrl;                             /**< <tt>\b 0x0004: </tt> - RTCCFG_CLK_CTRL - RTC Clock Control Settings                */
    __R  uint32_t rsv008;                               /**< <tt>\b 0x0008: </tt> - RESERVED                                                    */
    __IO uint32_t osc_ctrl;                             /**< <tt>\b 0x000C: </tt> - RTCCFG_OSC_CTRL - RTC Oscillator Control                    */
} mxc_rtccfg_regs_t;
/**@} end of group rtc_registers.*/

/*
   Register offsets for module RTC.
*/
/**
 * @defgroup   RTC_Register_Offsets Register Offsets
 * @ingroup    rtc_registers
 * @brief      Real-Time Clock Register Offsets from the RTC Base Peripheral Address.
 * @{
 */
#define MXC_R_RTCTMR_OFFS_CTRL                              ((uint32_t)0x00000000UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0000</tt> */
#define MXC_R_RTCTMR_OFFS_TIMER                             ((uint32_t)0x00000004UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0004</tt> */
#define MXC_R_RTCTMR_OFFS_COMP0                             ((uint32_t)0x00000008UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0008</tt> */
#define MXC_R_RTCTMR_OFFS_COMP1                             ((uint32_t)0x0000000CUL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x000C</tt> */
#define MXC_R_RTCTMR_OFFS_FLAGS                             ((uint32_t)0x00000010UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0010</tt> */
#define MXC_R_RTCTMR_OFFS_SNZ_VAL                           ((uint32_t)0x00000014UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0014</tt> */
#define MXC_R_RTCTMR_OFFS_INTEN                             ((uint32_t)0x00000018UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0018</tt> */
#define MXC_R_RTCTMR_OFFS_PRESCALE                          ((uint32_t)0x0000001CUL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x001C</tt> */
#define MXC_R_RTCTMR_OFFS_PRESCALE_MASK                     ((uint32_t)0x00000024UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0024</tt> */
#define MXC_R_RTCTMR_OFFS_TRIM_CTRL                         ((uint32_t)0x00000028UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0028</tt> */
#define MXC_R_RTCTMR_OFFS_TRIM_VALUE                        ((uint32_t)0x0000002CUL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x002C</tt> */
/**@} end of group RTC_Register_Offsets */
/**
 * @defgroup   RTCCFG_Register_Offsets RTCCFG Register Offsets
 * @ingroup    rtc_registers
 * @brief      Real-Time Clock CFG Register Offsets from the RTCCFG Base Peripheral Address.
 * @{
 */
#define MXC_R_RTCCFG_OFFS_NANO_CNTR                         ((uint32_t)0x00000000UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0000</tt> */
#define MXC_R_RTCCFG_OFFS_CLK_CTRL                          ((uint32_t)0x00000004UL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x0004</tt> */
#define MXC_R_RTCCFG_OFFS_OSC_CTRL                          ((uint32_t)0x0000000CUL)    /**< Offset from the RTC Base Peripheral Address:<tt>\b 0x000C</tt> */
/**@} end of group RTCCFG_Register_Offsets */

/*
   Field positions and masks for module RTC.
*/
/**
 * @defgroup RTC_CTRL_Register RTC_CTRL
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_CTRL_ENABLE_POS                           0                                                                           /**< ENABLE Position */
#define MXC_F_RTC_CTRL_ENABLE                               ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_ENABLE_POS))                     /**< ENABLE Mask */
#define MXC_F_RTC_CTRL_CLEAR_POS                            1                                                                           /**< CLEAR Position */
#define MXC_F_RTC_CTRL_CLEAR                                ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_CLEAR_POS))                      /**< CLEAR Mask */
#define MXC_F_RTC_CTRL_PENDING_POS                          2                                                                           /**< PENDING Position */
#define MXC_F_RTC_CTRL_PENDING                              ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_PENDING_POS))                    /**< PENDING Mask */
#define MXC_F_RTC_CTRL_USE_ASYNC_FLAGS_POS                  3                                                                           /**< USE_ASYNC_FLAGS Position */
#define MXC_F_RTC_CTRL_USE_ASYNC_FLAGS                      ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_USE_ASYNC_FLAGS_POS))            /**< USE_ASYNC_FLAGS Mask */
#define MXC_F_RTC_CTRL_AGGRESSIVE_RST_POS                   4                                                                           /**< AGGRESSIVE_RST Position */
#define MXC_F_RTC_CTRL_AGGRESSIVE_RST                       ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_AGGRESSIVE_RST_POS))             /**< AGGRESSIVE_RST Mask */
#define MXC_F_RTC_CTRL_AUTO_UPDATE_DISABLE_POS              5                                                                           /**< AUTO_UPDATE_DISABLE Position */
#define MXC_F_RTC_CTRL_AUTO_UPDATE_DISABLE                  ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_AUTO_UPDATE_DISABLE_POS))        /**< AUTO_UPDATE_DISABLE Mask */
#define MXC_F_RTC_CTRL_SNOOZE_ENABLE_POS                    6                                                                           /**< SNOOZE_ENABLE Position */
#define MXC_F_RTC_CTRL_SNOOZE_ENABLE                        ((uint32_t)(0x00000003UL << MXC_F_RTC_CTRL_SNOOZE_ENABLE_POS))              /**< SNOOZE_ENABLE Mask */
#define MXC_F_RTC_CTRL_RTC_ENABLE_ACTIVE_POS                16                                                                          /**< RTC_ENABLE_ACTIVE Position */
#define MXC_F_RTC_CTRL_RTC_ENABLE_ACTIVE                    ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_RTC_ENABLE_ACTIVE_POS))          /**< RTC_ENABLE_ACTIVE Mask */
#define MXC_F_RTC_CTRL_OSC_GOTO_LOW_ACTIVE_POS              17                                                                          /**< OSC_GOTO_LOW_ACTIVE Position */
#define MXC_F_RTC_CTRL_OSC_GOTO_LOW_ACTIVE                  ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_OSC_GOTO_LOW_ACTIVE_POS))        /**< OSC_GOTO_LOW_ACTIVE Mask */
#define MXC_F_RTC_CTRL_OSC_FRCE_SM_EN_ACTIVE_POS            18                                                                          /**< OSC_FRCE_SM_EN_ACTIVE Position */
#define MXC_F_RTC_CTRL_OSC_FRCE_SM_EN_ACTIVE                ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_OSC_FRCE_SM_EN_ACTIVE_POS))      /**< OSC_FRCE_SM_EN_ACTIVE Mask */
#define MXC_F_RTC_CTRL_OSC_FRCE_ST_ACTIVE_POS               19                                                                          /**< OSC_FRCE_ST_ACTIVE  Position */
#define MXC_F_RTC_CTRL_OSC_FRCE_ST_ACTIVE                   ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_OSC_FRCE_ST_ACTIVE_POS))         /**< OSC_FRCE_ST_ACTIVE Mask */
#define MXC_F_RTC_CTRL_RTC_SET_ACTIVE_POS                   20                                                                          /**< RTC_SET_ACTIVE Position */
#define MXC_F_RTC_CTRL_RTC_SET_ACTIVE                       ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_RTC_SET_ACTIVE_POS))             /**< RTC_SET_ACTIVE Mask */
#define MXC_F_RTC_CTRL_RTC_CLR_ACTIVE_POS                   21                                                                          /**< RTC_CLR_ACTIVE Position */
#define MXC_F_RTC_CTRL_RTC_CLR_ACTIVE                       ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_RTC_CLR_ACTIVE_POS))             /**< RTC_CLR_ACTIVE Mask */
#define MXC_F_RTC_CTRL_ROLLOVER_CLR_ACTIVE_POS              22                                                                          /**< ROLLOVER_CLR_ACTIVE Position */
#define MXC_F_RTC_CTRL_ROLLOVER_CLR_ACTIVE                  ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_ROLLOVER_CLR_ACTIVE_POS))        /**< ROLLOVER_CLR_ACTIVE Mask */
#define MXC_F_RTC_CTRL_PRESCALE_CMPR0_ACTIVE_POS            23                                                                          /**< PRESCALE_CMPR0_ACTIVE Position */
#define MXC_F_RTC_CTRL_PRESCALE_CMPR0_ACTIVE                ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_PRESCALE_CMPR0_ACTIVE_POS))      /**< PRESCALE_CMPR0_ACTIVE Mask */
#define MXC_F_RTC_CTRL_PRESCALE_UPDATE_ACTIVE_POS           24                                                                          /**< PRESCALE_UPDATE_ACTIVE Position */
#define MXC_F_RTC_CTRL_PRESCALE_UPDATE_ACTIVE               ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_PRESCALE_UPDATE_ACTIVE_POS))     /**< PRESCALE_UPDATE_ACTIVE Mask */
#define MXC_F_RTC_CTRL_CMPR1_CLR_ACTIVE_POS                 25                                                                          /**< CMPR1_CLR_ACTIVE Position */
#define MXC_F_RTC_CTRL_CMPR1_CLR_ACTIVE                     ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_CMPR1_CLR_ACTIVE_POS))           /**< CMPR1_CLR_ACTIVE Mask */
#define MXC_F_RTC_CTRL_CMPR0_CLR_ACTIVE_POS                 26                                                                          /**< CMPR0_CLR_ACTIVE Position */
#define MXC_F_RTC_CTRL_CMPR0_CLR_ACTIVE                     ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_CMPR0_CLR_ACTIVE_POS))           /**< CMPR0_CLR_ACTIVE Mask */
#define MXC_F_RTC_CTRL_TRIM_ENABLE_ACTIVE_POS               27                                                                          /**< TRIM_ENABLE_ACTIVE Position */
#define MXC_F_RTC_CTRL_TRIM_ENABLE_ACTIVE                   ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_TRIM_ENABLE_ACTIVE_POS))         /**< TRIM_ENABLE_ACTIVE Mask */
#define MXC_F_RTC_CTRL_TRIM_SLOWER_ACTIVE_POS               28                                                                          /**< TRIM_SLOWER_ACTIVE  Position */
#define MXC_F_RTC_CTRL_TRIM_SLOWER_ACTIVE                   ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_TRIM_SLOWER_ACTIVE_POS))         /**< TRIM_SLOWER_ACTIVE Mask */
#define MXC_F_RTC_CTRL_TRIM_CLR_ACTIVE_POS                  29                                                                          /**< TRIM_CLR_ACTIVE Position */
#define MXC_F_RTC_CTRL_TRIM_CLR_ACTIVE                      ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_TRIM_CLR_ACTIVE_POS))            /**< TRIM_CLR_ACTIVE Mask */
#define MXC_F_RTC_CTRL_ACTIVE_TRANS_0_POS                   30                                                                          /**< ACTIVE_TRANS_0 Position */
#define MXC_F_RTC_CTRL_ACTIVE_TRANS_0                       ((uint32_t)(0x00000001UL << MXC_F_RTC_CTRL_ACTIVE_TRANS_0_POS))             /**< ACTIVE_TRANS_0 Mask */
/**@} end of group RTC_CTRL*/
/**
 * @defgroup RTC_FLAGS_Register RTC_FLAGS
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_FLAGS_COMP0_POS                           0                                                                           /**< COMP0 Position */
#define MXC_F_RTC_FLAGS_COMP0                               ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_COMP0_POS))                     /**< COMP0 Mask */
#define MXC_F_RTC_FLAGS_COMP1_POS                           1                                                                           /**< COMP1 Position */
#define MXC_F_RTC_FLAGS_COMP1                               ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_COMP1_POS))                     /**< COMP1 Mask */
#define MXC_F_RTC_FLAGS_PRESCALE_COMP_POS                   2                                                                           /**< PRESCALE_COMP Position */
#define MXC_F_RTC_FLAGS_PRESCALE_COMP                       ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_PRESCALE_COMP_POS))             /**< PRESCALE_COMP Mask */
#define MXC_F_RTC_FLAGS_OVERFLOW_POS                        3                                                                           /**< OVERFLOW Position */
#define MXC_F_RTC_FLAGS_OVERFLOW                            ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_OVERFLOW_POS))                  /**< OVERFLOW Mask */
#define MXC_F_RTC_FLAGS_TRIM_POS                            4                                                                           /**< TRIM Position */
#define MXC_F_RTC_FLAGS_TRIM                                ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_TRIM_POS))                      /**< TRIM Mask */
#define MXC_F_RTC_FLAGS_SNOOZE_POS                          5                                                                           /**< SNOOZE Position */
#define MXC_F_RTC_FLAGS_SNOOZE                              ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_SNOOZE_POS))                    /**< SNOOZE Mask */
#define MXC_F_RTC_FLAGS_COMP0_FLAG_A_POS                    8                                                                           /**< COMP0_FLAG_A Position */
#define MXC_F_RTC_FLAGS_COMP0_FLAG_A                        ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_COMP0_FLAG_A_POS))              /**< COMP0_FLAG_A Mask */
#define MXC_F_RTC_FLAGS_COMP1_FLAG_A_POS                    9                                                                           /**< COMP1_FLAG_A Position */
#define MXC_F_RTC_FLAGS_COMP1_FLAG_A                        ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_COMP1_FLAG_A_POS))              /**< COMP1_FLAG_A Mask */
#define MXC_F_RTC_FLAGS_PRESCL_FLAG_A_POS                   10                                                                          /**< PRESCL_FLAG_A Position */
#define MXC_F_RTC_FLAGS_PRESCL_FLAG_A                       ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_PRESCL_FLAG_A_POS))             /**< PRESCL_FLAG_A Mask */
#define MXC_F_RTC_FLAGS_OVERFLOW_FLAG_A_POS                 11                                                                          /**< OVERFLOW_FLAG_A Position */
#define MXC_F_RTC_FLAGS_OVERFLOW_FLAG_A                     ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_OVERFLOW_FLAG_A_POS))           /**< OVERFLOW_FLAG_A Mask */
#define MXC_F_RTC_FLAGS_TRIM_FLAG_A_POS                     12                                                                          /**< TRIM_FLAG_A Position */
#define MXC_F_RTC_FLAGS_TRIM_FLAG_A                         ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_TRIM_FLAG_A_POS))               /**< TRIM_FLAG_A Mask */
#define MXC_F_RTC_FLAGS_SNOOZE_A_POS                        28                                                                          /**< SNOOZE_A Position */
#define MXC_F_RTC_FLAGS_SNOOZE_A                            ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_SNOOZE_A_POS))                  /**< SNOOZE_A Mask */
#define MXC_F_RTC_FLAGS_SNOOZE_B_POS                        29                                                                          /**< SNOOZE_B Position */
#define MXC_F_RTC_FLAGS_SNOOZE_B                            ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_SNOOZE_B_POS))                  /**< SNOOZE_B Mask */
#define MXC_F_RTC_FLAGS_ASYNC_CLR_FLAGS_POS                 31                                                                          /**< ASYNC_CLR_FLAGS Position */
#define MXC_F_RTC_FLAGS_ASYNC_CLR_FLAGS                     ((uint32_t)(0x00000001UL << MXC_F_RTC_FLAGS_ASYNC_CLR_FLAGS_POS))           /**< ASYNC_CLR_FLAGS Mask */
/**@} end of group RTC_FLAGS_Register */
/**
 * @defgroup RTC_SNZ_VAL_Register RTC_SNZ_VAL.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_SNZ_VAL_VALUE_POS                         0                                                                           /**< VALUE Position */
#define MXC_F_RTC_SNZ_VAL_VALUE                             ((uint32_t)(0x000003FFUL << MXC_F_RTC_SNZ_VAL_VALUE_POS))                   /**< VALUE Mask */
/**@} end of group RTC_SNZ_VAL_Register */
/**
 * @defgroup RTC_INTEN_Register RTC_INTEN.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_INTEN_COMP0_POS                           0                                                                           /**< COMP0 Position */
#define MXC_F_RTC_INTEN_COMP0                               ((uint32_t)(0x00000001UL << MXC_F_RTC_INTEN_COMP0_POS))                     /**< COMP0 Mask */
#define MXC_F_RTC_INTEN_COMP1_POS                           1                                                                           /**< COMP1 Position */
#define MXC_F_RTC_INTEN_COMP1                               ((uint32_t)(0x00000001UL << MXC_F_RTC_INTEN_COMP1_POS))                     /**< COMP1 Mask */
#define MXC_F_RTC_INTEN_PRESCALE_COMP_POS                   2                                                                           /**< PRESCALE_COMP Position */
#define MXC_F_RTC_INTEN_PRESCALE_COMP                       ((uint32_t)(0x00000001UL << MXC_F_RTC_INTEN_PRESCALE_COMP_POS))             /**< PRESCALE_COMP Mask */
#define MXC_F_RTC_INTEN_OVERFLOW_POS                        3                                                                           /**< OVERFLOW Position */
#define MXC_F_RTC_INTEN_OVERFLOW                            ((uint32_t)(0x00000001UL << MXC_F_RTC_INTEN_OVERFLOW_POS))                  /**< OVERFLOW Mask */
#define MXC_F_RTC_INTEN_TRIM_POS                            4                                                                           /**< TRIM Position */
#define MXC_F_RTC_INTEN_TRIM                                ((uint32_t)(0x00000001UL << MXC_F_RTC_INTEN_TRIM_POS))                      /**< TRIM Mask */
/**@} end of group RTC_INTEN_Register */
/**
 * @defgroup RTC_PRESCALE_Register RTC_PRESCALE.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_PRESCALE_PRESCALE_POS                     0                                                                           /**< PRESCALE Position */
#define MXC_F_RTC_PRESCALE_PRESCALE                         ((uint32_t)(0x0000000FUL << MXC_F_RTC_PRESCALE_PRESCALE_POS))               /**< PRESCALE Mask */
/**@} end of group RTC_INTEN_Register */
/**
 * @defgroup RTC_PRESCALE_MASK_Register RTC_PRESCALE_MASK.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_PRESCALE_MASK_PRESCALE_MASK_POS           0                                                                           /**< PRESCALE_MASK Position */
#define MXC_F_RTC_PRESCALE_MASK_PRESCALE_MASK               ((uint32_t)(0x0000000FUL << MXC_F_RTC_PRESCALE_MASK_PRESCALE_MASK_POS))     /**< PRESCALE_MASK Mask */
/**@} end of group RTC_PRESCALE_MASK_Register */
/**
 * @defgroup RTC_TRIM_CTRL_Register RTC_TRIM_CTRL.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_TRIM_CTRL_TRIM_ENABLE_R_POS               0                                                                           /**< TRIM_ENABLE_R Position */
#define MXC_F_RTC_TRIM_CTRL_TRIM_ENABLE_R                   ((uint32_t)(0x00000001UL << MXC_F_RTC_TRIM_CTRL_TRIM_ENABLE_R_POS))         /**< TRIM_ENABLE_R Mask */
#define MXC_F_RTC_TRIM_CTRL_TRIM_FASTER_OVR_R_POS           1                                                                           /**< TRIM_FASTER_OVR_R Position */
#define MXC_F_RTC_TRIM_CTRL_TRIM_FASTER_OVR_R               ((uint32_t)(0x00000001UL << MXC_F_RTC_TRIM_CTRL_TRIM_FASTER_OVR_R_POS))     /**< TRIM_FASTER_OVR_R Mask */
#define MXC_F_RTC_TRIM_CTRL_TRIM_SLOWER_R_POS               2                                                                           /**< TRIM_SLOWER_R Position */
#define MXC_F_RTC_TRIM_CTRL_TRIM_SLOWER_R                   ((uint32_t)(0x00000001UL << MXC_F_RTC_TRIM_CTRL_TRIM_SLOWER_R_POS))         /**< TRIM_SLOWER_R Mask */
/**@} end of group RTC_TRIM_CTRL_Register */
/**
 * @defgroup RTC_TRIM_VALUE_Register RTC_TRIM_VALUE.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_TRIM_VALUE_TRIM_VALUE_POS                 0                                                                           /**< TRIM_VALUE Position */
#define MXC_F_RTC_TRIM_VALUE_TRIM_VALUE                     ((uint32_t)(0x0003FFFFUL << MXC_F_RTC_TRIM_VALUE_TRIM_VALUE_POS))           /**< TRIM_VALUE Mask */
#define MXC_F_RTC_TRIM_VALUE_TRIM_SLOWER_CONTROL_POS        18                                                                          /**< TRIM_SLOWER_CONTROL Position */
#define MXC_F_RTC_TRIM_VALUE_TRIM_SLOWER_CONTROL            ((uint32_t)(0x00000001UL << MXC_F_RTC_TRIM_VALUE_TRIM_SLOWER_CONTROL_POS))  /**< TRIM_SLOWER_CONTROL Mask */
/**@} end of group RTC_TRIM_VALUE_Register */
/**
 * @defgroup RTC_NANO_CNTR_Register RTC_NANO_CNTR.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_NANO_CNTR_NANORING_COUNTER_POS            0                                                                           /**< NANORING_COUNTER Position */
#define MXC_F_RTC_NANO_CNTR_NANORING_COUNTER                ((uint32_t)(0x0000FFFFUL << MXC_F_RTC_NANO_CNTR_NANORING_COUNTER_POS))      /**< NANORING_COUNTER Mask */
/**@} end of group RTC_NANO_CNTR_Register */
/**
 * @defgroup RTC_CLK_CTRL_Register RTC_CLK_CTRL.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_CLK_CTRL_OSC1_EN_POS                      0                                                                           /**< OSC1_EN Position */
#define MXC_F_RTC_CLK_CTRL_OSC1_EN                          ((uint32_t)(0x00000001UL << MXC_F_RTC_CLK_CTRL_OSC1_EN_POS))                /**< OSC1_EN Mask */
#define MXC_F_RTC_CLK_CTRL_OSC2_EN_POS                      1                                                                           /**< OSC2_EN Position */
#define MXC_F_RTC_CLK_CTRL_OSC2_EN                          ((uint32_t)(0x00000001UL << MXC_F_RTC_CLK_CTRL_OSC2_EN_POS))                /**< OSC2_EN Mask */
#define MXC_F_RTC_CLK_CTRL_NANO_EN_POS                      2                                                                           /**< NANO_EN Position */
#define MXC_F_RTC_CLK_CTRL_NANO_EN                          ((uint32_t)(0x00000001UL << MXC_F_RTC_CLK_CTRL_NANO_EN_POS))                /**< NANO_EN Mask */
/**@} end of group RTC_CLK_CTRL_Register */
/**
 * @defgroup RTC_OSC_CTRL_Register RTC_OSC_CTRL.
 * @ingroup rtc_registers
 * @{
 */
#define MXC_F_RTC_OSC_CTRL_OSC_BYPASS_POS                   0                                                                           /**< OSC_BYPASS  Position */
#define MXC_F_RTC_OSC_CTRL_OSC_BYPASS                       ((uint32_t)(0x00000001UL << MXC_F_RTC_OSC_CTRL_OSC_BYPASS_POS))             /**< OSC_BYPASS Mask */
#define MXC_F_RTC_OSC_CTRL_OSC_DISABLE_R_POS                1                                                                           /**< OSC_DISABLE_R Position */
#define MXC_F_RTC_OSC_CTRL_OSC_DISABLE_R                    ((uint32_t)(0x00000001UL << MXC_F_RTC_OSC_CTRL_OSC_DISABLE_R_POS))          /**< OSC_DISABLE_R Mask */
#define MXC_F_RTC_OSC_CTRL_OSC_DISABLE_SEL_POS              2                                                                           /**< OSC_DISABLE_SEL Position */
#define MXC_F_RTC_OSC_CTRL_OSC_DISABLE_SEL                  ((uint32_t)(0x00000001UL << MXC_F_RTC_OSC_CTRL_OSC_DISABLE_SEL_POS))        /**< OSC_DISABLE_SEL Mask */
#define MXC_F_RTC_OSC_CTRL_OSC_DISABLE_O_POS                3                                                                           /**< OSC_DISABLE_O Position */
#define MXC_F_RTC_OSC_CTRL_OSC_DISABLE_O                    ((uint32_t)(0x00000001UL << MXC_F_RTC_OSC_CTRL_OSC_DISABLE_O_POS))          /**< OSC_DISABLE_O Mask */
#define MXC_F_RTC_OSC_CTRL_OSC_WARMUP_ENABLE_POS            14                                                                          /**< OSC_WARMUP_ENABLE Position */
#define MXC_F_RTC_OSC_CTRL_OSC_WARMUP_ENABLE                ((uint32_t)(0x00000001UL << MXC_F_RTC_OSC_CTRL_OSC_WARMUP_ENABLE_POS))      /**< OSC_WARMUP_ENABLE Mask */
/**@} end of group RTC_OSC_CTRL_Register */

/*
   Field values
*/
/**
 * @defgroup rtc_snz_mode_values RTC SNOOZE MODE Values
 * @ingroup RTC_CTRL_Register
 * @{
 */
#define MXC_V_RTC_CTRL_SNOOZE_DISABLE                       ((uint32_t)(0x00000000UL))  /**< SNOOZE Mode Disable */
#define MXC_V_RTC_CTRL_SNOOZE_MODE_A                        ((uint32_t)(0x00000001UL))  /**< SNOOZE Mode A */
#define MXC_V_RTC_CTRL_SNOOZE_MODE_B                        ((uint32_t)(0x00000002UL))  /**< SNOOZE Mode B */
/**@} end of group rtc_snz_mode_values */
/**
 * @defgroup rtc_prescale_values RTC Prescale Values
 * @ingroup RTC_PRESCALE_Register
 * @{
 */
#define MXC_V_RTC_PRESCALE_DIV_2_0                          ((uint32_t)(0x00000000UL))  /**< RTC Prescale Divide by \f$ 2^{0} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_1                          ((uint32_t)(0x00000001UL))  /**< RTC Prescale Divide by \f$ 2^{1} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_2                          ((uint32_t)(0x00000002UL))  /**< RTC Prescale Divide by \f$ 2^{2} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_3                          ((uint32_t)(0x00000003UL))  /**< RTC Prescale Divide by \f$ 2^{3} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_4                          ((uint32_t)(0x00000004UL))  /**< RTC Prescale Divide by \f$ 2^{4} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_5                          ((uint32_t)(0x00000005UL))  /**< RTC Prescale Divide by \f$ 2^{5} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_6                          ((uint32_t)(0x00000006UL))  /**< RTC Prescale Divide by \f$ 2^{6} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_7                          ((uint32_t)(0x00000007UL))  /**< RTC Prescale Divide by \f$ 2^{7} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_8                          ((uint32_t)(0x00000008UL))  /**< RTC Prescale Divide by \f$ 2^{8} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_9                          ((uint32_t)(0x00000009UL))  /**< RTC Prescale Divide by \f$ 2^{9} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_10                         ((uint32_t)(0x0000000AUL))  /**< RTC Prescale Divide by \f$ 2^{10} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_11                         ((uint32_t)(0x0000000BUL))  /**< RTC Prescale Divide by \f$ 2^{11} \f$.*/
#define MXC_V_RTC_PRESCALE_DIV_2_12                         ((uint32_t)(0x0000000CUL))  /**< RTC Prescale Divide by \f$ 2^{12} \f$.*/
/**@} end of group rtc_prescale_values*/

#ifdef __cplusplus
}
#endif

#endif   /* _MXC_RTC_REGS_H_ */
