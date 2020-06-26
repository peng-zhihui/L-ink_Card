/**
  ******************************************************************************
  * @file    x_nucleo_nfc04a1_nfctag.c
  * @author  MMY Application Team
  * @version $Revision: 3306 $
  * @date    $Date: 2017-01-13 11:18:15 +0100 (Fri, 13 Jan 2017) $
  * @brief   This file provides a set of functions needed to manage a nfc dual 
  *          interface eeprom memory.
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

/* Includes ------------------------------------------------------------------*/
#include "nfc04a1_nfctag.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup X_NUCLEO_NFC04A1
 * @{
 */

/** @defgroup X_NUCLEO_NFC04A1_NFCTAG
 * @{
 */
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup X_NUCLEO_NFC04A1_NFCTAG_Private_Defines
 * @{
 */
#ifndef NULL
#define NULL      (void *) 0
#endif
/**
 * @}
 */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
/* Global variables ----------------------------------------------------------*/
/** @defgroup X_NUCLEO_NFC04A1_NFCTAG_Private_Variables
 * @{
 */
static NFCTAG_DrvTypeDef *Nfctag_Drv = NULL;
/* static uint8_t NfctagInitialized = 0; */
static ST25DV_Object_t NfcTagObj;

/**
 * @}
 */
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
/** @defgroup X_NUCLEO_NFC04A1_NFCTAG_Public_Functions
 * @{
 */


int32_t NFC04A1_NFCTAG_Init (uint32_t Instance)
{
  int32_t status;
  ST25DV_IO_t IO;
  UNUSED(Instance);

  /* Configure the component */
  IO.Init         = NFC04A1_I2C_Init;
  IO.DeInit       = NFC04A1_I2C_DeInit;
  IO.IsReady      = NFC04A1_I2C_IsReady;
  IO.Read         = NFC04A1_I2C_ReadReg16;
  IO.Write        = (ST25DV_Write_Func)NFC04A1_I2C_WriteReg16;
  IO.GetTick      = NFC04A1_GetTick;

  status = ST25DV_RegisterBusIO (&NfcTagObj, &IO);
  if(status != NFCTAG_OK)
    return NFCTAG_ERROR;

  Nfctag_Drv = (NFCTAG_DrvTypeDef *)(void *)&St25Dv_Drv;
  if(Nfctag_Drv->Init != NULL)
  {
    status = Nfctag_Drv->Init(&NfcTagObj);
    if(status != NFCTAG_OK)
    {
      Nfctag_Drv = NULL;
      return NFCTAG_ERROR;
    }
  } else {
    Nfctag_Drv = NULL;
    return NFCTAG_ERROR;
  }
  return NFCTAG_OK;
}


/**
  * @brief  Deinitializes peripherals used by the I2C NFCTAG driver
  * @param  None
  * @retval None
  */
void NFC04A1_NFCTAG_DeInit( uint32_t Instance )
{ 
  UNUSED(Instance);

  if(Nfctag_Drv != NULL)
  {
    Nfctag_Drv = NULL;
    NfcTagObj.IsInitialized = 0;
  }
}

/**
  * @brief  Check if the nfctag is initialized
  * @param  None
  * @retval 0 if the nfctag is not initialized, 1 if the nfctag is already initialized
  */
uint8_t NFC04A1_NFCTAG_isInitialized( uint32_t Instance )
{
  int32_t status;
  UNUSED(Instance);
   status = NfcTagObj.IsInitialized;
   return status;
}

/**
  * @brief  Read the ID of the nfctag
  * @param  wai_id : the pointer where the who_am_i of the device is stored
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_ReadID( uint32_t Instance, uint8_t * const wai_id )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->ReadID == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->ReadID(&NfcTagObj, wai_id );
}

/**
  * @brief  Check if the nfctag is available
  * @param  Trials : Number of trials
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_IsDeviceReady( uint32_t Instance, const uint32_t Trials )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->IsReady == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->IsReady(&NfcTagObj, Trials );
}

/**
  * @brief  Configure nfctag interrupt
  * @param  ITConfig : store interrupt to configure
  *                  - 0x01 => RF BUSY
  *                  - 0x02 => WIP
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_ConfigIT( uint32_t Instance, const uint16_t ITConfig )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->ConfigIT == NULL )
  {
    return NFCTAG_ERROR;
  }
  return Nfctag_Drv->ConfigIT(&NfcTagObj, ITConfig );
}

/**
  * @brief  Get nfctag interrupt configutration
  * @param  ITConfig : store interrupt configuration
  *                  - 0x01 => RF BUSY
  *                  - 0x02 => WIP
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_GetITStatus(uint32_t Instance,  uint16_t * const ITConfig )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->GetITStatus == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->GetITStatus(&NfcTagObj, ITConfig );
}

/**
  * @brief  Reads data in the nfctag at specific address
  * @param  pData : pointer to store read data
  * @param  TarAddr : I2C data memory address to read
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_ReadData( uint32_t Instance, uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->ReadData == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->ReadData(&NfcTagObj, pData, TarAddr, Size );
}

/**
  * @brief  Writes data in the nfctag at specific address
  * @param  pData : pointer to the data to write
  * @param  TarAddr : I2C data memory address to write
  * @param  Size : Size in bytes of the value to be written
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_WriteData( uint32_t Instance, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  UNUSED(Instance);
  if ( Nfctag_Drv->WriteData == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->WriteData(&NfcTagObj, pData, TarAddr, Size );
}

/**
  * @brief  Reads nfctag Register
  * @param  pData : pointer to store read data
  * @param  TarAddr : I2C register address to read
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_ReadRegister( uint32_t Instance, uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  UNUSED(Instance);

  return ST25DV_ReadRegister(&NfcTagObj, pData, TarAddr, Size );
}

/**
  * @brief  Writes nfctag Register
  * @param  pData : pointer to the data to write
  * @param  TarAddr : I2C register address to write
  * @param  Size : Size in bytes of the value to be written
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_WriteRegister( uint32_t Instance, const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size )
{
  UNUSED(Instance);
  int32_t ret_value;

  ret_value = ST25DV_WriteRegister(&NfcTagObj, pData, TarAddr, Size );
  if( ret_value == NFCTAG_OK )
  {
    while( Nfctag_Drv->IsReady(&NfcTagObj, 1 ) != NFCTAG_OK ) {};
      return NFCTAG_OK;
  }
  
  return ret_value;
}

/**
  * @brief  Return the size of the nfctag
  * @retval Size of the NFCtag in Bytes
  */
uint32_t NFC04A1_NFCTAG_GetByteSize( uint32_t Instance )
{
  UNUSED(Instance);
  ST25DV_MEM_SIZE mem_size;
  ST25DV_ReadMemSize(&NfcTagObj, &mem_size );
  
  return (mem_size.BlockSize+1) * (mem_size.Mem_Size+1);
}

/**
  * @brief  Reads the ST25DV IC Revision.
  * @param  pICRev Pointer on the uint8_t used to return the ST25DV IC Revision number.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadICRev( uint32_t Instance, uint8_t * const pICRev )
{
  UNUSED(Instance);
  return ST25DV_ReadICRev(&NfcTagObj, pICRev);
}


/**
  * @brief  Reads the ST25DV ITtime duration for the GPO pulses.
  * @param  pITtime Pointer used to return the coefficient for the GPO Pulse duration (Pulse duration = 302,06 us - ITtime * 512 / fc).
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadITPulse(uint32_t Instance, ST25DV_PULSE_DURATION * const pITtime )
{
  UNUSED(Instance);
  return ST25DV_ReadITPulse(&NfcTagObj, pITtime);
}

/**
  * @brief    Configures the ST25DV ITtime duration for the GPO pulse.
  * @details  Needs the I2C Password presentation to be effective.
  * @param    ITtime Coefficient for the Pulse duration to be written (Pulse duration = 302,06 us - ITtime * 512 / fc)
  * @retval   int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteITPulse( uint32_t Instance, const ST25DV_PULSE_DURATION ITtime )
{
  UNUSED(Instance);
  return ST25DV_WriteITPulse(&NfcTagObj, ITtime);
}

/**
  * @brief  Reads the ST25DV UID.
  * @param  pUid Pointer used to return the ST25DV UID value.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadUID( uint32_t Instance, ST25DV_UID * const pUid )
{
  UNUSED(Instance);
  return ST25DV_ReadUID(&NfcTagObj, pUid);
}

/**
  * @brief  Reads the ST25DV DSFID.
  * @param  pDsfid Pointer used to return the ST25DV DSFID value.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadDSFID( uint32_t Instance, uint8_t * const pDsfid )
{
  UNUSED(Instance);
  return ST25DV_ReadDSFID(&NfcTagObj, pDsfid);
}

/**
  * @brief  Reads the ST25DV DSFID RF Lock state.
  * @param  pLockDsfid Pointer on a ST25DV_LOCK_STATUS used to return the DSFID lock state.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadDsfidRFProtection( uint32_t Instance, ST25DV_LOCK_STATUS * const pLockDsfid )
{
  UNUSED(Instance);
  return ST25DV_ReadDsfidRFProtection(&NfcTagObj, pLockDsfid);
}

/**
  * @brief  Reads the ST25DV AFI.
  * @param  pAfi Pointer used to return the ST25DV AFI value.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadAFI( uint32_t Instance, uint8_t * const pAfi )
{
  UNUSED(Instance);
  return ST25DV_ReadAFI(&NfcTagObj, pAfi);
}

/**
  * @brief  Reads the AFI RF Lock state.
  * @param  pLockAfi Pointer on a ST25DV_LOCK_STATUS used to return the ASFID lock state.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadAfiRFProtection( uint32_t Instance, ST25DV_LOCK_STATUS * const pLockAfi )
{
  UNUSED(Instance);
  return ST25DV_ReadAfiRFProtection(&NfcTagObj, pLockAfi);
}

/**
  * @brief  Reads the I2C Protected Area state.
  * @param  pProtZone Pointer on a ST25DV_I2C_PROT_ZONE structure used to return the Protected Area state.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadI2CProtectZone( uint32_t Instance, ST25DV_I2C_PROT_ZONE * const pProtZone )
{
  UNUSED(Instance);
  return ST25DV_ReadI2CProtectZone(&NfcTagObj, pProtZone);
}

/**
  * @brief    Sets the I2C write-protected state to an EEPROM Area.
  * @details  Needs the I2C Password presentation to be effective.
  * @param    Zone                ST25DV_PROTECTION_ZONE value coresponding to the area to protect.
  * @param    ReadWriteProtection ST25DV_PROTECTION_CONF value corresponding to the protection to be set.
  * @return   int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteI2CProtectZonex(uint32_t Instance, const ST25DV_PROTECTION_ZONE Zone,  const ST25DV_PROTECTION_CONF ReadWriteProtection )
{
  UNUSED(Instance);
  return ST25DV_WriteI2CProtectZonex(&NfcTagObj, Zone, ReadWriteProtection);
}

/**
  * @brief  Reads the CCile protection state.
  * @param  pLockCCFile Pointer on a ST25DV_LOCK_CCFILE value corresponding to the lock state of the CCFile.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadLockCCFile(uint32_t Instance, ST25DV_LOCK_CCFILE * const pLockCCFile )
{
  UNUSED(Instance);
  return ST25DV_ReadLockCCFile(&NfcTagObj, pLockCCFile);
}

/**
  * @brief  Locks the CCile to prevent any RF write access.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  NbBlockCCFile ST25DV_CCFILE_BLOCK value corresponding to the number of blocks to be locked.
  * @param  LockCCFile    ST25DV_LOCK_CCFILE value corresponding to the lock state to apply on the CCFile.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteLockCCFile(uint32_t Instance, const ST25DV_CCFILE_BLOCK NbBlockCCFile,  const ST25DV_LOCK_STATUS LockCCFile )
{
  UNUSED(Instance);
  return ST25DV_WriteLockCCFile(&NfcTagObj, NbBlockCCFile, LockCCFile);
}

/**
  * @brief  Reads the Cfg registers protection.
  * @param  pLockCfg Pointer on a ST25DV_LOCK_STATUS value corresponding to the Cfg registers lock state.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadLockCFG(uint32_t Instance, ST25DV_LOCK_STATUS * const pLockCfg )
{
  UNUSED(Instance);
  return ST25DV_ReadLockCFG(&NfcTagObj, pLockCfg);
}

/**
  * @brief  Lock/Unlock the Cfg registers, to prevent any RF write access.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  LockCfg ST25DV_LOCK_STATUS value corresponding to the lock state to be written.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteLockCFG(uint32_t Instance, const ST25DV_LOCK_STATUS LockCfg )
{
  UNUSED(Instance);
  return ST25DV_WriteLockCFG(&NfcTagObj, LockCfg);
}

/**
  * @brief  Presents I2C password, to authorize the I2C writes to protected areas.
  * @param  PassWord Password value on 32bits
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_PresentI2CPassword(uint32_t Instance, const ST25DV_PASSWD PassWord )
{
  UNUSED(Instance);
  return ST25DV_PresentI2CPassword(&NfcTagObj, PassWord);
}

/**
  * @brief  Writes a new I2C password.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  PassWord New I2C PassWord value on 32bits.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteI2CPassword(uint32_t Instance, const ST25DV_PASSWD PassWord )
{
  UNUSED(Instance);
  return ST25DV_WriteI2CPassword(&NfcTagObj, PassWord);
}

/**
  * @brief  Reads the RF Zone Security Status (defining the allowed RF accesses).
  * @param  Zone        ST25DV_PROTECTION_ZONE value coresponding to the protected area.
  * @param  pRfprotZone Pointer on a ST25DV_RF_PROT_ZONE value corresponding to the area protection state.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadRFZxSS(uint32_t Instance, const ST25DV_PROTECTION_ZONE Zone,  ST25DV_RF_PROT_ZONE * const pRfprotZone )
{
  UNUSED(Instance);
  return ST25DV_ReadRFZxSS(&NfcTagObj, Zone, pRfprotZone);
}

/**
  * @brief  Writes the RF Zone Security Status (defining the allowed RF accesses)
  * @details  Needs the I2C Password presentation to be effective.
  * @param  Zone        ST25DV_PROTECTION_ZONE value corresponding to the area on which to set the RF protection.
  * @param  RfProtZone  Pointer on a ST25DV_RF_PROT_ZONE value defininf the protection to be set on the area.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteRFZxSS(uint32_t Instance, const ST25DV_PROTECTION_ZONE Zone,  const ST25DV_RF_PROT_ZONE RfProtZone )
{
  UNUSED(Instance);
  return ST25DV_WriteRFZxSS(&NfcTagObj, Zone, RfProtZone);
}

/**
  * @brief  Reads the value of the an area end address.
  * @param  EndZone ST25DV_END_ZONE value corresponding to an area end address.
  * @param  pEndZ   Pointer used to return the end address of the area.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadEndZonex(uint32_t Instance, const ST25DV_END_ZONE EndZone,  uint8_t * const pEndZ )
{
  UNUSED(Instance);
  return ST25DV_ReadEndZonex(&NfcTagObj, EndZone, pEndZ);
}

/**
  * @brief    Sets the end address of an area.
  * @details  Needs the I2C Password presentation to be effective.
  * @note     The ST25DV answers a NACK when setting the EndZone2 & EndZone3 to same value than repectively EndZone1 & EndZone2.\n
  *           These NACKs are ok.
  * @param  EndZone ST25DV_END_ZONE value corresponding to an area.
  * @param  EndZ   End zone value to be written.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteEndZonex(uint32_t Instance, const ST25DV_END_ZONE EndZone,  const uint8_t EndZ )
{
  UNUSED(Instance);
  return ST25DV_WriteEndZonex(&NfcTagObj, EndZone, EndZ);
}

/**
  * @brief  Initializes the end address of the ST25DV areas with their default values (end of memory).
  * @details  Needs the I2C Password presentation to be effective..
  *           The ST25DV answers a NACK when setting the EndZone2 & EndZone3 to same value than repectively EndZone1 & EndZone2.
  *           These NACKs are ok.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_InitEndZone(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_InitEndZone(&NfcTagObj);
}

/**
  * @brief  Creates user areas with defined lengths.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  Zone1Length Length of area1 in bytes (32 to 8192, 0x20 to 0x2000)
  * @param  Zone2Length Length of area2 in bytes (0 to 8128, 0x00 to 0x1FC0)
  * @param  Zone3Length Length of area3 in bytes (0 to 8064, 0x00 to 0x1F80)
  * @param  Zone4Length Length of area4 in bytes (0 to 8000, 0x00 to 0x1F40)
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_CreateUserZone(uint32_t Instance, uint16_t Zone1Length,  uint16_t Zone2Length,  uint16_t Zone3Length,  uint16_t Zone4Length )
{
  UNUSED(Instance);
  return ST25DV_CreateUserZone(&NfcTagObj, Zone1Length, Zone2Length, Zone3Length, Zone4Length);
}

/**
  * @brief  Reads the ST25DV Memory Size.
  * @param  pSizeInfo Pointer on a ST25DV_MEM_SIZE structure used to return the Memory size information.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadMemSize(uint32_t Instance, ST25DV_MEM_SIZE * const pSizeInfo )
{
  UNUSED(Instance);
  return ST25DV_ReadMemSize(&NfcTagObj, pSizeInfo);
}

/**
  * @brief  Reads the Energy harvesting mode.
  * @param  pEH_mode Pointer on a ST25DV_EH_MODE_STATUS value corresponding to the Energy Harvesting state.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadEHMode(uint32_t Instance, ST25DV_EH_MODE_STATUS * const pEH_mode )
{
  UNUSED(Instance);
  return ST25DV_ReadEHMode(&NfcTagObj, pEH_mode);
}

/**
  * @brief  Sets the Energy harvesting mode.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  EH_mode ST25DV_EH_MODE_STATUS value for the Energy harvesting mode to be set.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteEHMode(uint32_t Instance, const ST25DV_EH_MODE_STATUS EH_mode )
{
  UNUSED(Instance);
  return ST25DV_WriteEHMode(&NfcTagObj, EH_mode);
}

/**
  * @brief  Reads the RF Management configuration.
  * @param  pRF_Mngt Pointer on a ST25DV_RF_MNGT structure used to return the RF Management configuration.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadRFMngt(uint32_t Instance, ST25DV_RF_MNGT * const pRF_Mngt )
{
  UNUSED(Instance);
  return ST25DV_ReadRFMngt(&NfcTagObj, pRF_Mngt);
}

/**
  * @brief  Sets the RF Management configuration.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  Rfmngt Value of the RF Management configuration to be written.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteRFMngt(uint32_t Instance, const uint8_t Rfmngt )
{
  UNUSED(Instance);
  return ST25DV_WriteRFMngt(&NfcTagObj, Rfmngt);
}

/**
  * @brief  Reads the RFDisable register information.
  * @param  pRFDisable Pointer on a ST25DV_EN_STATUS value corresponding to the RF Disable status.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_GetRFDisable(uint32_t Instance, ST25DV_EN_STATUS * const pRFDisable )
{
  UNUSED(Instance);
  return ST25DV_GetRFDisable(&NfcTagObj, pRFDisable);
}

/**
  * @brief  Sets the RF Disable configuration.
  * @details  Needs the I2C Password presentation to be effective.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_SetRFDisable(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_SetRFDisable(&NfcTagObj);
}

/**
  * @brief  Resets the RF Disable configuration
  * @details  Needs the I2C Password presentation to be effective.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ResetRFDisable(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_ResetRFDisable(&NfcTagObj);
}

/**
  * @brief  Reads the RFSleep register information.
  * @param  pRFSleep Pointer on a ST25DV_EN_STATUS value corresponding to the RF Sleep status.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_GetRFSleep(uint32_t Instance, ST25DV_EN_STATUS * const pRFSleep )
{
  UNUSED(Instance);
  return ST25DV_GetRFSleep(&NfcTagObj, pRFSleep);
}

/**
  * @brief  Sets the RF Sleep configuration.
  * @details  Needs the I2C Password presentation to be effective.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_SetRFSleep(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_SetRFSleep(&NfcTagObj);
}

/**
  * @brief  Resets the RF Sleep configuration.
  * @details  Needs the I2C Password presentation to be effective.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ResetRFSleep(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_ResetRFSleep(&NfcTagObj);
}

/**
  * @brief  Reads the Mailbox mode.
  * @param  pMB_mode Pointer on a ST25DV_EH_MODE_STATUS value used to return the Mailbox mode.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadMBMode(uint32_t Instance, ST25DV_EN_STATUS * const pMB_mode )
{
  UNUSED(Instance);
  return ST25DV_ReadMBMode(&NfcTagObj, pMB_mode);
}

/**
  * @brief  Sets the Mailbox mode.
  * @details  Needs the I2C Password presentation to be effective.
  * @param  MB_mode ST25DV_EN_STATUS value corresponding to the Mailbox mode to be set.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteMBMode(uint32_t Instance, const ST25DV_EN_STATUS MB_mode )
{
  UNUSED(Instance);
  return ST25DV_WriteMBMode(&NfcTagObj, MB_mode);
}

/**
  * @brief  Reads the Mailbox watchdog duration coefficient.
  * @param  pWdgDelay Pointer on a uint8_t used to return the watchdog duration coefficient.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadMBWDG(uint32_t Instance, uint8_t * const pWdgDelay )
{
  UNUSED(Instance);
  return ST25DV_ReadMBWDG(&NfcTagObj, pWdgDelay);
}

/**
  * @brief  Writes the Mailbox watchdog coefficient delay
  * @details  Needs the I2C Password presentation to be effective.
  * @param  WdgDelay Watchdog duration coefficient to be written (Watch dog duration = MB_WDG*30 ms +/- 6%).
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteMBWDG(uint32_t Instance, const uint8_t WdgDelay )
{
  UNUSED(Instance);
  return ST25DV_WriteMBWDG(&NfcTagObj, WdgDelay);
}

/**
  * @brief  Reads N bytes of data from the Mailbox, starting at the specified byte offset.
  * @param  pData   Pointer on the buffer used to return the read data.
  * @param  Offset  Offset in the Mailbox memory, byte number to start the read.
  * @param  NbByte  Number of bytes to be read.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadMailboxData(uint32_t Instance, uint8_t * const pData,  const uint16_t TarAddr,  const uint16_t NbByte )
{
  UNUSED(Instance);
  return ST25DV_ReadMailboxData(&NfcTagObj, pData, TarAddr, NbByte);
}

/**
  * @brief  Writes N bytes of data in the Mailbox, starting from first Mailbox Address.
  * @param  pData   Pointer to the buffer containing the data to be written.
  * @param  NbByte  Number of bytes to be written.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteMailboxData(uint32_t Instance, const uint8_t * const pData,  const uint16_t NbByte )
{
  UNUSED(Instance);
  return ST25DV_WriteMailboxData(&NfcTagObj, pData, NbByte);
}

/**
  * @brief  Reads N bytes from the mailbox registers, starting at the specified I2C address.
  * @param  pData   Pointer on the buffer used to return the data.
  * @param  TarAddr I2C memory address to be read.
  * @param  NbByte  Number of bytes to be read.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadMailboxRegister(uint32_t Instance, uint8_t * const pData,  const uint16_t TarAddr,  const uint16_t NbByte )
{
  UNUSED(Instance);
  return ST25DV_ReadMailboxRegister(&NfcTagObj, pData, TarAddr, NbByte);
}

/**
  * @brief  Writes N bytes to the specified mailbox register.
  * @param  pData   Pointer on the data to be written.
  * @param  TarAddr I2C register address to be written.
  * @param  NbByte  Number of bytes to be written.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteMailboxRegister(uint32_t Instance, const uint8_t * const pData,  const uint16_t TarAddr,  const uint16_t NbByte )
{
  UNUSED(Instance);
  return ST25DV_WriteMailboxRegister(&NfcTagObj, pData, TarAddr, NbByte);
}

/**
  * @brief  Reads the status of the security session open register.
  * @param  pSession Pointer on a ST25DV_I2CSSO_STATUS value used to return the session status.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(uint32_t Instance, ST25DV_I2CSSO_STATUS * const pSession )
{
  UNUSED(Instance);
  return ST25DV_ReadI2CSecuritySession_Dyn(&NfcTagObj, pSession);
}

/**
  * @brief  Reads the IT status register from the ST25DV.
  * @param  pITStatus Pointer on uint8_t, used to return the IT status, such as:
  *                       - RFUSERSTATE = 0x01
  *                       - RFBUSY = 0x02
  *                       - RFINTERRUPT = 0x04
  *                       - FIELDFALLING = 0x08
  *                       - FIELDRISING = 0x10
  *                       - RFPUTMSG = 0x20
  *                       - RFGETMSG = 0x40
  *                       - RFWRITE = 0x80
  *
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadITSTStatus_Dyn(uint32_t Instance, uint8_t * const pITStatus )
{
  UNUSED(Instance);
  return ST25DV_ReadITSTStatus_Dyn(&NfcTagObj, pITStatus);
}

/**
  * @brief  Read value of dynamic GPO register configuration.
  * @param  pGPO ST25DV_GPO pointer of the dynamic GPO configuration to store.
  * @retval NFCTAG enum status.
  */
int32_t NFC04A1_NFCTAG_ReadGPO_Dyn(uint32_t Instance, uint8_t *GPOConfig )
{
  UNUSED(Instance);
  return ST25DV_ReadGPO_Dyn(&NfcTagObj, GPOConfig);
}

/**
  * @brief  Get dynamique GPO enable status
  * @param  pGPO_en ST25DV_EN_STATUS pointer of the GPO enable status to store
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_GetGPO_en_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pGPO_en )
{
  UNUSED(Instance);
  return ST25DV_GetGPO_en_Dyn(&NfcTagObj, pGPO_en);
}

/**
  * @brief  Set dynamique GPO enable configuration.
  * @param  None No parameters.
  * @retval NFCTAG enum status.
  */
int32_t NFC04A1_NFCTAG_SetGPO_en_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_SetGPO_en_Dyn(&NfcTagObj);
}

/**
  * @brief  Reset dynamique GPO enable configuration.
  * @param  None No parameters.
  * @retval NFCTAG enum status.
  */
int32_t NFC04A1_NFCTAG_ResetGPO_en_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_ResetGPO_en_Dyn(&NfcTagObj);
}

/**
  * @brief  Read value of dynamic EH Ctrl register configuration
  * @param  pEH_CTRL : ST25DV_EH_CTRL pointer of the dynamic EH Ctrl configuration to store
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_ReadEHCtrl_Dyn(uint32_t Instance, ST25DV_EH_CTRL * const pEH_CTRL )
{
  UNUSED(Instance);
  return ST25DV_ReadEHCtrl_Dyn(&NfcTagObj, pEH_CTRL);
}

/**
  * @brief  Reads the Energy Harvesting dynamic status.
  * @param  pEH_Val Pointer on a ST25DV_EN_STATUS value used to return the Energy Harvesting dynamic status.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_GetEHENMode_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pEH_Val )
{
  UNUSED(Instance);
  return ST25DV_GetEHENMode_Dyn(&NfcTagObj, pEH_Val);
}

/**
  * @brief  Dynamically sets the Energy Harvesting mode.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_SetEHENMode_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_SetEHENMode_Dyn(&NfcTagObj);
}

/**
  * @brief  Dynamically unsets the Energy Harvesting mode.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ResetEHENMode_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_ResetEHENMode_Dyn(&NfcTagObj);
}

/**
  * @brief  Reads the EH_ON status from the EH_CTRL_DYN register.
  * @param  pEHON Pointer on a ST25DV_EN_STATUS value used to return the EHON status.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_GetEHON_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pEHON )
{
  UNUSED(Instance);
  return ST25DV_GetEHON_Dyn(&NfcTagObj, pEHON);
}

/**
  * @brief  Checks if RF Field is present in front of the ST25DV.
  * @param  pRF_Field Pointer on a ST25DV_FIELD_STATUS value used to return the field presence.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_GetRFField_Dyn(uint32_t Instance, ST25DV_FIELD_STATUS * const pRF_Field )
{
  UNUSED(Instance);
  return ST25DV_GetRFField_Dyn(&NfcTagObj, pRF_Field);
}

/**
  * @brief  Check if VCC is supplying the ST25DV.
  * @param  pVCC ST25DV_VCC_STATUS pointer of the VCC status to store
  * @retval NFCTAG enum status.
  */
int32_t NFC04A1_NFCTAG_GetVCC_Dyn(uint32_t Instance, ST25DV_VCC_STATUS * const pVCC )
{
  UNUSED(Instance);
  return ST25DV_GetVCC_Dyn(&NfcTagObj, pVCC);
}

/**
  * @brief  Read value of dynamic RF Management configuration
  * @param  pRF_Mngt : ST25DV_RF_MNGT pointer of the dynamic RF Management configuration to store
  * @retval NFCTAG enum status
  */
int32_t NFC04A1_NFCTAG_ReadRFMngt_Dyn(uint32_t Instance, ST25DV_RF_MNGT * const pRF_Mngt )
{
  UNUSED(Instance);
  return ST25DV_ReadRFMngt_Dyn(&NfcTagObj, pRF_Mngt);
}

/**
  * @brief  Writes a value to the RF Management dynamic register.
  * @param  RF_Mngt Value to be written to the RF Management dynamic register.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_WriteRFMngt_Dyn(uint32_t Instance, const uint8_t RF_Mngt )
{
  UNUSED(Instance);
  return ST25DV_WriteRFMngt_Dyn(&NfcTagObj, RF_Mngt);
}

/**
  * @brief  Reads the RFDisable dynamic register information.
  * @param  pRFDisable Pointer on a ST25DV_EN_STATUS value used to return the RF Disable state.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_GetRFDisable_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pRFDisable )
{
  UNUSED(Instance);
  return ST25DV_GetRFDisable_Dyn(&NfcTagObj, pRFDisable);
}

/**
  * @brief  Sets the RF Disable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_SetRFDisable_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_SetRFDisable_Dyn(&NfcTagObj);
}

/**
  * @brief  Unsets the RF Disable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ResetRFDisable_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_ResetRFDisable_Dyn(&NfcTagObj);
}

/**
  * @brief  Reads the RFSleep dynamic register information.
  * @param  pRFSleep Pointer on a ST25DV_EN_STATUS values used to return the RF Sleep state.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_GetRFSleep_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pRFSleep )
{
  UNUSED(Instance);
  return ST25DV_GetRFSleep_Dyn(&NfcTagObj, pRFSleep);
}

/**
  * @brief  Sets the RF Sleep dynamic configuration.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_SetRFSleep_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_SetRFSleep_Dyn(&NfcTagObj);
}

/**
  * @brief  Unsets the RF Sleep dynamic configuration.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ResetRFSleep_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_ResetRFSleep_Dyn(&NfcTagObj);
}

/**
  * @brief  Reads the Mailbox ctrl dynamic register.
  * @param  pCtrlStatus Pointer on a ST25DV_MB_CTRL_DYN_STATUS structure used to return the dynamic Mailbox ctrl information.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadMBCtrl_Dyn(uint32_t Instance, ST25DV_MB_CTRL_DYN_STATUS * const pCtrlStatus )
{
  UNUSED(Instance);
  return ST25DV_ReadMBCtrl_Dyn(&NfcTagObj, pCtrlStatus);
}

/**
  * @brief  Reads the Mailbox Enable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_GetMBEN_Dyn(uint32_t Instance, ST25DV_EN_STATUS * const pMBEN )
{
  UNUSED(Instance);
  return ST25DV_GetMBEN_Dyn(&NfcTagObj, pMBEN);
}

/**
  * @brief  Sets the Mailbox Enable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_SetMBEN_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_SetMBEN_Dyn(&NfcTagObj);
}

/**
  * @brief  Unsets the Mailbox Enable dynamic configuration.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ResetMBEN_Dyn(uint32_t Instance)
{
  UNUSED(Instance);
  return ST25DV_ResetMBEN_Dyn(&NfcTagObj);
}

/**
  * @brief  Reads the Mailbox message length dynamic register.
  * @param  pMBLength Pointer on a uint8_t used to return the Mailbox message length.
  * @return int32_t enum status.
  */
int32_t NFC04A1_NFCTAG_ReadMBLength_Dyn(uint32_t Instance, uint8_t * const pMBLength )
{
  UNUSED(Instance);
  return ST25DV_ReadMBLength_Dyn(&NfcTagObj, pMBLength);
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

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
