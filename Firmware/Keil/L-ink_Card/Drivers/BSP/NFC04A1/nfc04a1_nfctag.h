/**
  ******************************************************************************
  * @file    x_nucleo_nfc04a1_nfctag.h
  * @author  MMY Application Team
  * @version $Revision: 2983 $
  * @date    $Date: 2016-09-27 15:08:30 +0200 (Tue, 27 Sep 2016) $
  * @brief   This file contains definitions for the x_nucleo_nfc04a1_nfctag.c
  *          specific functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
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
#ifndef __X_NUCLEO_NFC04A1_NFCTAG_H
#define __X_NUCLEO_NFC04A1_NFCTAG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nfc04a1.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup X_NUCLEO_NFC04A1
  * @{
  */
   
/** @addtogroup X_NUCLEO_NFC04A1_NFCTAG
  * @{
  */
/* Exported types ------------------------------------------------------------*/
/**
 * @brief  NFCTAG status enumerator definition.
 */


/* Exported constants --------------------------------------------------------*/
#define NFCTAG_4K_SIZE            ((uint32_t) 0x200)
#define NFCTAG_16K_SIZE           ((uint32_t) 0x800)
#define NFCTAG_64K_SIZE           ((uint32_t) 0x2000)
/* External variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function	--------------------------------------------------------*/
/** @defgroup X_NUCLEO_NFC04A1_NFCTAG_Exported_Functions
  * @{
  */
int32_t NFC04A1_NFCTAG_Init( uint32_t Instance );
void NFC04A1_NFCTAG_DeInit( uint32_t Instance );
uint8_t NFC04A1_NFCTAG_isInitialized( uint32_t Instance );
int32_t NFC04A1_NFCTAG_ReadID( uint32_t Instance, uint8_t * const wai_id );
int32_t NFC04A1_NFCTAG_ConfigIT( uint32_t Instance, const uint16_t ITConfig );
int32_t NFC04A1_NFCTAG_GetITStatus( uint32_t Instance, uint16_t * const ITConfig );
int32_t NFC04A1_NFCTAG_ReadData( uint32_t Instance, uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );
int32_t NFC04A1_NFCTAG_WriteData( uint32_t Instance, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );
int32_t NFC04A1_NFCTAG_ReadRegister( uint32_t Instance, uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );
int32_t NFC04A1_NFCTAG_WriteRegister( uint32_t Instance, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size );
int32_t NFC04A1_NFCTAG_IsDeviceReady( uint32_t Instance,const uint32_t Trials );

uint32_t NFC04A1_NFCTAG_GetByteSize( uint32_t Instance );
int32_t NFC04A1_NFCTAG_ReadICRev( uint32_t Instance, uint8_t * const pICRev );
int32_t NFC04A1_NFCTAG_ReadITPulse( uint32_t Instance, ST25DV_PULSE_DURATION * const pITtime );
int32_t NFC04A1_NFCTAG_WriteITPulse( uint32_t Instance, const ST25DV_PULSE_DURATION ITtime );
int32_t NFC04A1_NFCTAG_ReadUID( uint32_t Instance, ST25DV_UID * const pUid );
int32_t NFC04A1_NFCTAG_ReadDSFID( uint32_t Instance, uint8_t * const pDsfid );
int32_t NFC04A1_NFCTAG_ReadDsfidRFProtection( uint32_t Instance, ST25DV_LOCK_STATUS * const pLockDsfid );
int32_t NFC04A1_NFCTAG_ReadAFI( uint32_t Instance, uint8_t * const pAfi );
int32_t NFC04A1_NFCTAG_ReadAfiRFProtection( uint32_t Instance, ST25DV_LOCK_STATUS * const pLockAfi );
int32_t NFC04A1_NFCTAG_ReadI2CProtectZone( uint32_t Instance, ST25DV_I2C_PROT_ZONE * const pProtZone );
int32_t NFC04A1_NFCTAG_WriteI2CProtectZonex(uint32_t Instance, const ST25DV_PROTECTION_ZONE Zone,  const ST25DV_PROTECTION_CONF ReadWriteProtection );
int32_t NFC04A1_NFCTAG_ReadLockCCFile(uint32_t Instance, ST25DV_LOCK_CCFILE * const pLockCCFile );
int32_t NFC04A1_NFCTAG_WriteLockCCFile(uint32_t Instance, const ST25DV_CCFILE_BLOCK NbBlockCCFile,  const ST25DV_LOCK_STATUS LockCCFile );
int32_t NFC04A1_NFCTAG_ReadLockCFG(uint32_t Instance, ST25DV_LOCK_STATUS * const pLockCfg );
int32_t NFC04A1_NFCTAG_WriteLockCFG(uint32_t Instance, const ST25DV_LOCK_STATUS LockCfg );
int32_t NFC04A1_NFCTAG_PresentI2CPassword(uint32_t Instance, const ST25DV_PASSWD PassWord );
int32_t NFC04A1_NFCTAG_WriteI2CPassword(uint32_t Instance, const ST25DV_PASSWD PassWord );
int32_t NFC04A1_NFCTAG_ReadRFZxSS(uint32_t Instance, const ST25DV_PROTECTION_ZONE Zone,  ST25DV_RF_PROT_ZONE * const pRfprotZone );
int32_t NFC04A1_NFCTAG_WriteRFZxSS(uint32_t Instance, const ST25DV_PROTECTION_ZONE Zone,  const ST25DV_RF_PROT_ZONE RfProtZone );
int32_t NFC04A1_NFCTAG_ReadEndZonex(uint32_t Instance, const ST25DV_END_ZONE EndZone,  uint8_t * const pEndZ );
int32_t NFC04A1_NFCTAG_WriteEndZonex(uint32_t Instance, const ST25DV_END_ZONE EndZone,  const uint8_t EndZ );
int32_t NFC04A1_NFCTAG_InitEndZone(uint32_t Instance);
int32_t NFC04A1_NFCTAG_CreateUserZone(uint32_t Instance, uint16_t Zone1Length,  uint16_t Zone2Length,  uint16_t Zone3Length,  uint16_t Zone4Length );
int32_t NFC04A1_NFCTAG_ReadMemSize(uint32_t Instance, ST25DV_MEM_SIZE * const pSizeInfo );
int32_t NFC04A1_NFCTAG_ReadEHMode(uint32_t Instance, ST25DV_EH_MODE_STATUS * const pEH_mode );
int32_t NFC04A1_NFCTAG_WriteEHMode(uint32_t Instance, const ST25DV_EH_MODE_STATUS EH_mode );
int32_t NFC04A1_NFCTAG_ReadRFMngt(uint32_t Instance, ST25DV_RF_MNGT * const pRF_Mngt );
int32_t NFC04A1_NFCTAG_WriteRFMngt(uint32_t Instance, const uint8_t Rfmngt );
int32_t NFC04A1_NFCTAG_GetRFDisable(uint32_t Instance, ST25DV_EN_STATUS * const pRFDisable );
int32_t NFC04A1_NFCTAG_SetRFDisable(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ResetRFDisable(uint32_t Instance);
int32_t NFC04A1_NFCTAG_GetRFSleep(uint32_t Instance, ST25DV_EN_STATUS * const pRFSleep );
int32_t NFC04A1_NFCTAG_SetRFSleep(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ResetRFSleep(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ReadMBMode(uint32_t Instance, ST25DV_EN_STATUS * const pMB_mode );
int32_t NFC04A1_NFCTAG_WriteMBMode(uint32_t Instance, const ST25DV_EN_STATUS MB_mode );
int32_t NFC04A1_NFCTAG_ReadMBWDG(uint32_t Instance, uint8_t * const pWdgDelay );
int32_t NFC04A1_NFCTAG_WriteMBWDG(uint32_t Instance, const uint8_t WdgDelay );
int32_t NFC04A1_NFCTAG_ReadMailboxData(uint32_t Instance, uint8_t * const pData,  const uint16_t TarAddr,  const uint16_t NbByte );
int32_t NFC04A1_NFCTAG_WriteMailboxData(uint32_t Instance, const uint8_t * const pData,  const uint16_t NbByte );
int32_t NFC04A1_NFCTAG_ReadMailboxRegister(uint32_t Instance, uint8_t * const pData,  const uint16_t TarAddr,  const uint16_t NbByte );
int32_t NFC04A1_NFCTAG_WriteMailboxRegister(uint32_t Instance, const uint8_t * const pData,  const uint16_t TarAddr,  const uint16_t NbByte );
int32_t NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(uint32_t Instance, ST25DV_I2CSSO_STATUS * const pSession );
int32_t NFC04A1_NFCTAG_ReadITSTStatus_Dyn(uint32_t Instance, uint8_t * const pITStatus );
int32_t NFC04A1_NFCTAG_ReadGPO_Dyn(uint32_t Instance, uint8_t *GPOConfig );
int32_t NFC04A1_NFCTAG_GetGPO_en_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pGPO_en );
int32_t NFC04A1_NFCTAG_SetGPO_en_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ResetGPO_en_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ReadEHCtrl_Dyn(uint32_t Instance, ST25DV_EH_CTRL * const pEH_CTRL );
int32_t NFC04A1_NFCTAG_GetEHENMode_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pEH_Val );
int32_t NFC04A1_NFCTAG_SetEHENMode_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ResetEHENMode_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_GetEHON_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pEHON );
int32_t NFC04A1_NFCTAG_GetRFField_Dyn(uint32_t Instance, ST25DV_FIELD_STATUS * const pRF_Field );
int32_t NFC04A1_NFCTAG_GetVCC_Dyn(uint32_t Instance, ST25DV_VCC_STATUS * const pVCC );
int32_t NFC04A1_NFCTAG_ReadRFMngt_Dyn(uint32_t Instance, ST25DV_RF_MNGT * const pRF_Mngt );
int32_t NFC04A1_NFCTAG_WriteRFMngt_Dyn(uint32_t Instance, const uint8_t RF_Mngt );
int32_t NFC04A1_NFCTAG_GetRFDisable_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pRFDisable );
int32_t NFC04A1_NFCTAG_SetRFDisable_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ResetRFDisable_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_GetRFSleep_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pRFSleep );
int32_t NFC04A1_NFCTAG_SetRFSleep_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ResetRFSleep_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ReadMBCtrl_Dyn(uint32_t Instance, ST25DV_MB_CTRL_DYN_STATUS * const pCtrlStatus );
int32_t NFC04A1_NFCTAG_GetMBEN_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pMBEN );
int32_t NFC04A1_NFCTAG_SetMBEN_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ResetMBEN_Dyn(uint32_t Instance);
int32_t NFC04A1_NFCTAG_ReadMBLength_Dyn(uint32_t Instance, uint8_t * const pMBLength );



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

#endif /* __X_NUCLEO_NFC04A1_NFCTAG_H */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
