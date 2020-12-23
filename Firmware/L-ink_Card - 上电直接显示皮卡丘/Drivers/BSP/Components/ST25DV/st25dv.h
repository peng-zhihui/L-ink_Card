/**
  ******************************************************************************
  * @file    st25dv.h
  * @author  MMY Application Team
  * @version $Revision: 3308 $
  * @date    $Date: 2017-01-13 11:19:33 +0100 (Fri, 13 Jan 2017) $
  * @brief   This file provides set of driver functions to manage communication 
  * @brief   between MCU and ST25DV chip 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under ST MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/myliberty  
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ST25DV_H
#define __ST25DV_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "st25dv_reg.h"

/** @addtogroup BSP
  * @{
  */ 
 

/** @addtogroup ST25DV
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief  ST25DV Enable Disable enumerator definition.
 */
typedef enum
{
  ST25DV_DISABLE = 0,
  ST25DV_ENABLE
} ST25DV_EN_STATUS;

/**
 * @brief  ST25DV Energy Harvesting mode enumerator definition.
 */
typedef enum
{
  ST25DV_EH_ACTIVE_AFTER_BOOT = 0,
  ST25DV_EH_ON_DEMAND
} ST25DV_EH_MODE_STATUS;

/**
 * @brief  ST25DV FIELD status enumerator definition.
 */
typedef enum
{
  ST25DV_FIELD_OFF = 0,
  ST25DV_FIELD_ON
} ST25DV_FIELD_STATUS;

/**
 * @brief  ST25DV VCC status enumerator definition
 */
typedef enum
{
  ST25DV_VCC_OFF = 0,
  ST25DV_VCC_ON
} ST25DV_VCC_STATUS;

/**
 * @brief  ST25DV protection status enumerator definition
 */
typedef enum
{
  ST25DV_NO_PROT = 0,
  ST25DV_WRITE_PROT,
  ST25DV_READ_PROT,
  ST25DV_READWRITE_PROT
} ST25DV_PROTECTION_CONF;

/**
 * @brief  ST25DV area protection enumerator definition.
 */
typedef enum
{
  ST25DV_PROT_ZONE1 = 0,
  ST25DV_PROT_ZONE2,
  ST25DV_PROT_ZONE3,
  ST25DV_PROT_ZONE4
} ST25DV_PROTECTION_ZONE;

/**
 * @brief  ST25DV password protection status enumerator definition.
 */
typedef enum
{
  ST25DV_NOT_PROTECTED = 0,
  ST25DV_PROT_PASSWD1,
  ST25DV_PROT_PASSWD2,
  ST25DV_PROT_PASSWD3
} ST25DV_PASSWD_PROT_STATUS;

/**
 * @brief  ST25DV lock status enumerator definition.
 */
typedef enum
{
  ST25DV_UNLOCKED = 0,
  ST25DV_LOCKED
} ST25DV_LOCK_STATUS;

/**
 * @brief  ST25DV Number of Blocks for the CCFile enumerator definition.
 */
typedef enum
{
  ST25DV_CCFILE_1BLCK = 0,
  ST25DV_CCFILE_2BLCK
} ST25DV_CCFILE_BLOCK;

/**
 * @brief  ST25DV session status enumerator definition.
 */
typedef enum
{
  ST25DV_SESSION_CLOSED = 0,
  ST25DV_SESSION_OPEN
} ST25DV_I2CSSO_STATUS;

/**
 * @brief  ST25DV area end address enumerator definition.
 */
typedef enum
{
  ST25DV_ZONE_END1 = 0,
  ST25DV_ZONE_END2,
  ST25DV_ZONE_END3
} ST25DV_END_ZONE;

/**
 * @brief  ST25DV IT pulse duration enumerator definition.
 */
typedef enum
{
  ST25DV_302_US = 0,
  ST25DV_264_US,
  ST25DV_226_US,
  ST25DV_188_US,
  ST25DV_151_US,
  ST25DV_113_US,
  ST25DV_75_US,
  ST25DV_37_US  
} ST25DV_PULSE_DURATION;

/**
 * @brief  ST25DV Mailbox Current Message enumerator definition
 */
typedef enum
{
  ST25DV_NO_MSG = 0,
  ST25DV_HOST_MSG,
  ST25DV_RF_MSG  
} ST25DV_CURRENT_MSG;

/**
 * @brief  ST25DV EH Ctrl structure definition
 */
typedef struct
{
  ST25DV_EN_STATUS EH_EN_Mode;
  ST25DV_EN_STATUS EH_on;
  ST25DV_EN_STATUS Field_on;
  ST25DV_EN_STATUS VCC_on;
} ST25DV_EH_CTRL;

/**
 * @brief  ST25DV GPO structure definition
 */
typedef struct
{
  ST25DV_EN_STATUS GPO_RFUser_en;
  ST25DV_EN_STATUS GPO_RFActivity_en;
  ST25DV_EN_STATUS GPO_RFInterrupt_en;
  ST25DV_EN_STATUS GPO_FieldChange_en;
  ST25DV_EN_STATUS GPO_RFPutMsg_en;
  ST25DV_EN_STATUS GPO_RFGetMsg_en;
  ST25DV_EN_STATUS GPO_RFWrite_en;
  ST25DV_EN_STATUS GPO_Enable;
} ST25DV_GPO;

/**
 * @brief  ST25DV RF Management structure definition.
 */
typedef struct
{
  ST25DV_EN_STATUS RfDisable;
  ST25DV_EN_STATUS RfSleep;
} ST25DV_RF_MNGT;

/**
 * @brief  ST25DV RF Area protection structure definition.
 */
typedef struct
{
  ST25DV_PASSWD_PROT_STATUS PasswdCtrl;
  ST25DV_PROTECTION_CONF RWprotection;
} ST25DV_RF_PROT_ZONE;

/**
 * @brief  ST25DV I2C Area protection structure definition.
 */
typedef struct
{
  ST25DV_PROTECTION_CONF ProtectZone1;
  ST25DV_PROTECTION_CONF ProtectZone2;
  ST25DV_PROTECTION_CONF ProtectZone3;
  ST25DV_PROTECTION_CONF ProtectZone4;
} ST25DV_I2C_PROT_ZONE;

/**
 * @brief  ST25DV MB_CTRL_DYN register structure definition.
 */
typedef struct
{
  uint8_t MbEnable;
  uint8_t HostPutMsg;
  uint8_t RfPutMsg;
  uint8_t HostMissMsg;
  uint8_t RFMissMsg;
  ST25DV_CURRENT_MSG CurrentMsg;
} ST25DV_MB_CTRL_DYN_STATUS;

/**
 * @brief  ST25DV Lock CCFile structure definition.
 */
typedef struct
{
  ST25DV_LOCK_STATUS LckBck0;
  ST25DV_LOCK_STATUS LckBck1;
} ST25DV_LOCK_CCFILE;

/**
 * @brief  ST25DV Memory size structure definition.
 */
typedef struct
{
  uint8_t BlockSize;
  uint16_t Mem_Size;
} ST25DV_MEM_SIZE;

/**
 * @brief  ST25DV UID information structure definition.
 */
typedef struct
{
  uint32_t MsbUid;
  uint32_t LsbUid;
} ST25DV_UID;

/**
 * @brief  ST25DV Password structure definition.
 */
typedef struct
{
  uint32_t MsbPasswd;
  uint32_t LsbPasswd;
} ST25DV_PASSWD;


typedef int32_t (*ST25DV_Init_Func) (void);
typedef int32_t (*ST25DV_DeInit_Func) (void);
typedef uint32_t (*ST25DV_GetTick_Func) (void);
typedef int32_t (*ST25DV_Write_Func)(uint16_t, uint16_t, const uint8_t*, uint16_t);
typedef int32_t (*ST25DV_Read_Func) (uint16_t, uint16_t, uint8_t*, uint16_t);
typedef int32_t (*ST25DV_IsReady_Func) (uint16_t, const uint32_t);

typedef struct {
  ST25DV_Init_Func    Init;
  ST25DV_DeInit_Func  DeInit;
  ST25DV_IsReady_Func IsReady;
  ST25DV_Write_Func   Write;
  ST25DV_Read_Func    Read;
  ST25DV_GetTick_Func GetTick;
} ST25DV_IO_t;


typedef struct {
  ST25DV_IO_t IO ;
  ST25DV_Ctx_t Ctx ;
  uint32_t IsInitialized;

} ST25DV_Object_t;


#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief  NFCTAG standard driver API structure definition.
 */
typedef struct
{
  int32_t       (*Init)( void* );
  int32_t       (*ReadID)( void*, uint8_t * const );
  int32_t       (*IsReady)( void*, const uint32_t );
  int32_t       (*GetITStatus)( void*, uint16_t * const );
  int32_t       (*ConfigIT)( void*, const uint16_t );
  int32_t       (*ReadData)( void*, uint8_t * const, const uint16_t, const uint16_t );
  int32_t       (*WriteData)( void*, const uint8_t * const, const uint16_t, const uint16_t );
} NFCTAG_DrvTypeDef;

typedef struct
{
  int32_t       (*Init)( ST25DV_Object_t* );
  int32_t       (*ReadID)( ST25DV_Object_t*, uint8_t * const );
  int32_t       (*IsReady)( ST25DV_Object_t*, const uint32_t );
  int32_t       (*GetITStatus)( ST25DV_Object_t*, uint16_t * const );
  int32_t       (*ConfigIT)( ST25DV_Object_t*, const uint16_t );
  int32_t       (*ReadData)( ST25DV_Object_t*, uint8_t * const, const uint16_t, const uint16_t );
  int32_t       (*WriteData)( ST25DV_Object_t*, const uint8_t * const, const uint16_t, const uint16_t );
} ST25DV_Drv_t;


#endif

/* Exported constants --------------------------------------------------------*/
/** @brief ST25DV 4Kbits ICref */
#define I_AM_ST25DV04                        0x24
/** @brief ST25DV 16/64Kbits ICref */
#define I_AM_ST25DV64                        0x26

/** @brief Check ST25DV Open Drain version */
#define ST25DV_AM_I_OPEN_DRAIN(x)     (((x) == 0x26) || ((x) == 0x24))
/** @brief Check ST25DV CMOS version */
#define ST25DV_AM_I_CMOS(x)           (((x) == 0x27) || ((x) == 0x25))


#ifndef NULL
#define NULL      (void *) 0
#endif

/** @brief I2C address to be used for ST25DV Data accesses. */
#define ST25DV_ADDR_DATA_I2C                 0xA6
/** @brief I2C address to be used for ST25DV System accesses. */
#define ST25DV_ADDR_SYST_I2C                 0xAE

/** @brief I2C Time out (ms), min value : (Max write bytes) / (Internal page write) * tw   (256/4)*5. */
#define ST25DV_WRITE_TIMEOUT                   320 

/** @brief Size of the ST25DV write buffer. */
#define ST25DV_MAX_WRITE_BYTE                256
/** @brief Size of the ST25DVMailbox memory. */
#define ST25DV_MAX_MAILBOX_LENGTH            256

/** @brief Offset of ST25DV dynamic registers. */
#define ST25DV_IS_DYNAMIC_REGISTER          0x2000

  
/* External variables --------------------------------------------------------*/
/* NFCTAG driver structure */
extern ST25DV_Drv_t St25Dv_Drv;

/* Exported macro ------------------------------------------------------------*/
/* Imported functions ------------------------------------------------------- */

/* Exported functions ------------------------------------------------------- */
int32_t ST25DV_ReadRegister( ST25DV_Object_t*, uint8_t * const, const uint16_t, const uint16_t );
int32_t ST25DV_WriteRegister( ST25DV_Object_t*, const uint8_t * const, const uint16_t, const uint16_t );
int32_t ST25DV_RegisterBusIO (ST25DV_Object_t* pObj, ST25DV_IO_t *pIO);
int32_t ST25DV_ReadMemSize( ST25DV_Object_t* pObj, ST25DV_MEM_SIZE * const pSizeInfo );
int32_t ST25DV_ReadICRev(ST25DV_Object_t* pObj,  uint8_t * const pICRev );
int32_t ST25DV_ReadITPulse(ST25DV_Object_t* pObj, ST25DV_PULSE_DURATION * const pITtime );
int32_t ST25DV_WriteITPulse(ST25DV_Object_t* pObj,  const ST25DV_PULSE_DURATION ITtime );
int32_t ST25DV_ReadUID(ST25DV_Object_t* pObj,  ST25DV_UID * const pUid );
int32_t ST25DV_ReadDSFID(ST25DV_Object_t* pObj,  uint8_t * const pDsfid );
int32_t ST25DV_ReadDsfidRFProtection(ST25DV_Object_t* pObj,  ST25DV_LOCK_STATUS * const pLockDsfid );
int32_t ST25DV_ReadAFI(ST25DV_Object_t* pObj,  uint8_t * const pAfi );
int32_t ST25DV_ReadAfiRFProtection(ST25DV_Object_t* pObj,  ST25DV_LOCK_STATUS * const pLockAfi );
int32_t ST25DV_ReadI2CProtectZone(ST25DV_Object_t* pObj,  ST25DV_I2C_PROT_ZONE * const pProtZone );
int32_t ST25DV_WriteI2CProtectZonex( ST25DV_Object_t* pObj, const ST25DV_PROTECTION_ZONE Zone, const ST25DV_PROTECTION_CONF ReadWriteProtection );
int32_t ST25DV_ReadLockCCFile( ST25DV_Object_t* pObj, ST25DV_LOCK_CCFILE * const pLockCCFile );
int32_t ST25DV_WriteLockCCFile( ST25DV_Object_t* pObj, const ST25DV_CCFILE_BLOCK NbBlockCCFile, const ST25DV_LOCK_STATUS LockCCFile );
int32_t ST25DV_ReadLockCFG( ST25DV_Object_t* pObj, ST25DV_LOCK_STATUS * const pLockCfg );
int32_t ST25DV_WriteLockCFG( ST25DV_Object_t* pObj, const ST25DV_LOCK_STATUS LockCfg );
int32_t ST25DV_PresentI2CPassword( ST25DV_Object_t* pObj, const ST25DV_PASSWD PassWord );
int32_t ST25DV_WriteI2CPassword( ST25DV_Object_t* pObj, const ST25DV_PASSWD PassWord );
int32_t ST25DV_ReadRFZxSS( ST25DV_Object_t* pObj, const ST25DV_PROTECTION_ZONE Zone, ST25DV_RF_PROT_ZONE * const pRfprotZone );
int32_t ST25DV_WriteRFZxSS( ST25DV_Object_t* pObj, const ST25DV_PROTECTION_ZONE Zone, const ST25DV_RF_PROT_ZONE RfProtZone );
int32_t ST25DV_ReadEndZonex( ST25DV_Object_t* pObj, const ST25DV_END_ZONE EndZone, uint8_t * pEndZ );
int32_t ST25DV_WriteEndZonex( ST25DV_Object_t* pObj, const ST25DV_END_ZONE EndZone, const uint8_t EndZ );
int32_t ST25DV_InitEndZone( ST25DV_Object_t* pObj );
int32_t ST25DV_CreateUserZone( ST25DV_Object_t* pObj, uint16_t Zone1Length, uint16_t Zone2Length, uint16_t Zone3Length, uint16_t Zone4Length );
int32_t ST25DV_ReadMemSize( ST25DV_Object_t* pObj, ST25DV_MEM_SIZE * const pSizeInfo );
int32_t ST25DV_ReadEHMode( ST25DV_Object_t* pObj, ST25DV_EH_MODE_STATUS * const pEH_mode );
int32_t ST25DV_WriteEHMode( ST25DV_Object_t* pObj, const ST25DV_EH_MODE_STATUS EH_mode );
int32_t ST25DV_ReadRFMngt( ST25DV_Object_t* pObj, ST25DV_RF_MNGT * const pRF_Mngt );
int32_t ST25DV_WriteRFMngt( ST25DV_Object_t* pObj, const uint8_t Rfmngt );
int32_t ST25DV_GetRFDisable( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pRFDisable );
int32_t ST25DV_SetRFDisable( ST25DV_Object_t* pObj );
int32_t ST25DV_ResetRFDisable( ST25DV_Object_t* pObj );
int32_t ST25DV_GetRFSleep( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pRFSleep );
int32_t ST25DV_SetRFSleep( ST25DV_Object_t* pObj );
int32_t ST25DV_ResetRFSleep( ST25DV_Object_t* pObj );
int32_t ST25DV_ReadMBMode( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pMB_mode );
int32_t ST25DV_WriteMBMode( ST25DV_Object_t* pObj, const ST25DV_EN_STATUS MB_mode );
int32_t ST25DV_ReadMBWDG( ST25DV_Object_t* pObj, uint8_t * const pWdgDelay );
int32_t ST25DV_WriteMBWDG( ST25DV_Object_t* pObj, const uint8_t WdgDelay );
int32_t ST25DV_ReadMailboxData( ST25DV_Object_t* pObj, uint8_t * const pData, const uint16_t TarAddr, const uint16_t NbByte );
int32_t ST25DV_WriteMailboxData( ST25DV_Object_t* pObj, const uint8_t * const pData, const uint16_t NbByte );
int32_t ST25DV_ReadMailboxRegister( ST25DV_Object_t* pObj, uint8_t * const pData, const uint16_t TarAddr, const uint16_t NbByte );
int32_t ST25DV_WriteMailboxRegister( ST25DV_Object_t* pObj, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t NbByte );
int32_t ST25DV_ReadI2CSecuritySession_Dyn( ST25DV_Object_t* pObj, ST25DV_I2CSSO_STATUS * const pSession );
int32_t ST25DV_ReadITSTStatus_Dyn( ST25DV_Object_t* pObj, uint8_t * const pITStatus );
int32_t ST25DV_ReadGPO_Dyn( ST25DV_Object_t* pObj, uint8_t *GPOConfig );
int32_t ST25DV_GetGPO_en_Dyn( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pGPO_en );
int32_t ST25DV_SetGPO_en_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_ResetGPO_en_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_ReadEHCtrl_Dyn( ST25DV_Object_t* pObj, ST25DV_EH_CTRL * const pEH_CTRL );
int32_t ST25DV_GetEHENMode_Dyn( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pEH_Val );
int32_t ST25DV_SetEHENMode_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_ResetEHENMode_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_GetEHON_Dyn( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pEHON );
int32_t ST25DV_GetRFField_Dyn( ST25DV_Object_t* pObj, ST25DV_FIELD_STATUS * const pRF_Field );
int32_t ST25DV_GetVCC_Dyn( ST25DV_Object_t* pObj, ST25DV_VCC_STATUS * const pVCC );
int32_t ST25DV_ReadRFMngt_Dyn( ST25DV_Object_t* pObj, ST25DV_RF_MNGT * const pRF_Mngt );
int32_t ST25DV_WriteRFMngt_Dyn( ST25DV_Object_t* pObj, const uint8_t RF_Mngt );
int32_t ST25DV_GetRFDisable_Dyn( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pRFDisable );
int32_t ST25DV_SetRFDisable_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_ResetRFDisable_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_GetRFSleep_Dyn( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pRFSleep );
int32_t ST25DV_SetRFSleep_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_ResetRFSleep_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_ReadMBCtrl_Dyn( ST25DV_Object_t* pObj, ST25DV_MB_CTRL_DYN_STATUS * const pCtrlStatus );
int32_t ST25DV_GetMBEN_Dyn( ST25DV_Object_t* pObj, ST25DV_EN_STATUS * const pMBEN );
int32_t ST25DV_SetMBEN_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_ResetMBEN_Dyn( ST25DV_Object_t* pObj );
int32_t ST25DV_ReadMBLength_Dyn( ST25DV_Object_t* pObj, uint8_t * const pMBLength );

/**
  * @}
  */

/**
  * @}
  */ 
  

#ifdef __cplusplus
  }
#endif
#endif /* __ST25DV_H */

/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
