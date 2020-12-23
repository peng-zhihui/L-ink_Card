#include "st25dv_reg.h"

int32_t ST25DV_ReadReg (ST25DV_Ctx_t *ctx, uint16_t Reg, uint8_t* Data, uint16_t len)
{
  return ctx->ReadReg(ctx->handle, Reg, Data, len);
}

int32_t ST25DV_WriteReg (ST25DV_Ctx_t *ctx, uint16_t Reg, uint8_t const *Data, uint16_t len)
{
  return ctx->WriteReg(ctx->handle, Reg, Data, len);
}


/**** Copy generated code hereafter ****/
int32_t ST25DV_GetICREF (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ICREF_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetENDA1 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ENDA1_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_SetENDA1 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  return ST25DV_WriteReg(ctx, (ST25DV_ENDA1_REG), value, 1);
}

int32_t ST25DV_GetENDA2 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ENDA2_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_SetENDA2 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  return ST25DV_WriteReg(ctx, (ST25DV_ENDA2_REG), value, 1);
}

int32_t ST25DV_GetENDA3 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ENDA3_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_SetENDA3 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  return ST25DV_WriteReg(ctx, (ST25DV_ENDA3_REG), value, 1);
}

int32_t ST25DV_GetDSFID (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_DSFID_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetAFI (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_AFI_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetMEM_SIZE_MSB (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MEM_SIZE_MSB_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetBLK_SIZE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_BLK_SIZE_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetMEM_SIZE_LSB (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MEM_SIZE_LSB_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetICREV (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ICREV_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetUID (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_UID_REG), (uint8_t *)value, 8))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetI2CPASSWD (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CPASSWD_REG), (uint8_t *)value, 8))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_SetI2CPASSWD (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  if( ST25DV_WriteReg(ctx, (ST25DV_I2CPASSWD_REG), value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetLOCKDSFID (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKDSFID_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetLOCKAFI (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKAFI_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_MODE_RW (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_MODE_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_MODE_RW_MASK);
  *value = *value >> (ST25DV_MB_MODE_RW_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetMB_MODE_RW (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_MODE_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_MB_MODE_RW_SHIFT)) & (ST25DV_MB_MODE_RW_MASK)) |
                (reg_value & ~(ST25DV_MB_MODE_RW_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_MB_MODE_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetMBLEN_DYN_MBLEN (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MBLEN_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_CTRL_DYN_MBEN (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_CTRL_DYN_MBEN_MASK);
  *value = *value >> (ST25DV_MB_CTRL_DYN_MBEN_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetMB_CTRL_DYN_MBEN (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_MB_CTRL_DYN_MBEN_SHIFT)) & (ST25DV_MB_CTRL_DYN_MBEN_MASK)) |
                (reg_value & ~(ST25DV_MB_CTRL_DYN_MBEN_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_MB_CTRL_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_CTRL_DYN_HOSTPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_CTRL_DYN_HOSTPUTMSG_MASK);
  *value = *value >> (ST25DV_MB_CTRL_DYN_HOSTPUTMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_CTRL_DYN_RFPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_CTRL_DYN_RFPUTMSG_MASK);
  *value = *value >> (ST25DV_MB_CTRL_DYN_RFPUTMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_CTRL_DYN_STRESERVED (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_CTRL_DYN_STRESERVED_MASK);
  *value = *value >> (ST25DV_MB_CTRL_DYN_STRESERVED_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_CTRL_DYN_HOSTMISSMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_CTRL_DYN_HOSTMISSMSG_MASK);
  *value = *value >> (ST25DV_MB_CTRL_DYN_HOSTMISSMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_CTRL_DYN_RFMISSMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_CTRL_DYN_RFMISSMSG_MASK);
  *value = *value >> (ST25DV_MB_CTRL_DYN_RFMISSMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_CTRL_DYN_CURRENTMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_CTRL_DYN_CURRENTMSG_MASK);
  *value = *value >> (ST25DV_MB_CTRL_DYN_CURRENTMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_CTRL_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetMB_WDG_DELAY (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_WDG_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_MB_WDG_DELAY_MASK);
  *value = *value >> (ST25DV_MB_WDG_DELAY_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetMB_WDG_DELAY (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_MB_WDG_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_MB_WDG_DELAY_SHIFT)) & (ST25DV_MB_WDG_DELAY_MASK)) |
                (reg_value & ~(ST25DV_MB_WDG_DELAY_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_MB_WDG_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_RFUSERSTATE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_RFUSERSTATE_MASK);
  *value = *value >> (ST25DV_GPO_RFUSERSTATE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_RFUSERSTATE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_RFUSERSTATE_SHIFT)) & (ST25DV_GPO_RFUSERSTATE_MASK)) |
                (reg_value & ~(ST25DV_GPO_RFUSERSTATE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_RFACTIVITY (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_RFACTIVITY_MASK);
  *value = *value >> (ST25DV_GPO_RFACTIVITY_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_RFACTIVITY (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_RFACTIVITY_SHIFT)) & (ST25DV_GPO_RFACTIVITY_MASK)) |
                (reg_value & ~(ST25DV_GPO_RFACTIVITY_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_RFINTERRUPT (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_RFINTERRUPT_MASK);
  *value = *value >> (ST25DV_GPO_RFINTERRUPT_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_RFINTERRUPT (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_RFINTERRUPT_SHIFT)) & (ST25DV_GPO_RFINTERRUPT_MASK)) |
                (reg_value & ~(ST25DV_GPO_RFINTERRUPT_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_FIELDCHANGE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_FIELDCHANGE_MASK);
  *value = *value >> (ST25DV_GPO_FIELDCHANGE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_FIELDCHANGE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_FIELDCHANGE_SHIFT)) & (ST25DV_GPO_FIELDCHANGE_MASK)) |
                (reg_value & ~(ST25DV_GPO_FIELDCHANGE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_RFPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_RFPUTMSG_MASK);
  *value = *value >> (ST25DV_GPO_RFPUTMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_RFPUTMSG (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_RFPUTMSG_SHIFT)) & (ST25DV_GPO_RFPUTMSG_MASK)) |
                (reg_value & ~(ST25DV_GPO_RFPUTMSG_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_RFGETMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_RFGETMSG_MASK);
  *value = *value >> (ST25DV_GPO_RFGETMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_RFGETMSG (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_RFGETMSG_SHIFT)) & (ST25DV_GPO_RFGETMSG_MASK)) |
                (reg_value & ~(ST25DV_GPO_RFGETMSG_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_RFWRITE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_RFWRITE_MASK);
  *value = *value >> (ST25DV_GPO_RFWRITE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_RFWRITE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_RFWRITE_SHIFT)) & (ST25DV_GPO_RFWRITE_MASK)) |
                (reg_value & ~(ST25DV_GPO_RFWRITE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_ENABLE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_ENABLE_MASK);
  *value = *value >> (ST25DV_GPO_ENABLE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_ENABLE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_ENABLE_SHIFT)) & (ST25DV_GPO_ENABLE_MASK)) |
                (reg_value & ~(ST25DV_GPO_ENABLE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_REG), value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_RFUSERSTATE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_DYN_RFUSERSTATE_MASK);
  *value = *value >> (ST25DV_GPO_DYN_RFUSERSTATE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_RFUSERSTATE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_DYN_RFUSERSTATE_SHIFT)) & (ST25DV_GPO_DYN_RFUSERSTATE_MASK)) |
                (reg_value & ~(ST25DV_GPO_DYN_RFUSERSTATE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_RFACTIVITY (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_DYN_RFACTIVITY_MASK);
  *value = *value >> (ST25DV_GPO_DYN_RFACTIVITY_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_RFACTIVITY (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_DYN_RFACTIVITY_SHIFT)) & (ST25DV_GPO_DYN_RFACTIVITY_MASK)) |
                (reg_value & ~(ST25DV_GPO_DYN_RFACTIVITY_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_RFINTERRUPT (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_DYN_RFINTERRUPT_MASK);
  *value = *value >> (ST25DV_GPO_DYN_RFINTERRUPT_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_RFINTERRUPT (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_DYN_RFINTERRUPT_SHIFT)) & (ST25DV_GPO_DYN_RFINTERRUPT_MASK)) |
                (reg_value & ~(ST25DV_GPO_DYN_RFINTERRUPT_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_FIELDCHANGE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_DYN_FIELDCHANGE_MASK);
  *value = *value >> (ST25DV_GPO_DYN_FIELDCHANGE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_FIELDCHANGE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_DYN_FIELDCHANGE_SHIFT)) & (ST25DV_GPO_DYN_FIELDCHANGE_MASK)) |
                (reg_value & ~(ST25DV_GPO_DYN_FIELDCHANGE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_RFPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_DYN_RFPUTMSG_MASK);
  *value = *value >> (ST25DV_GPO_DYN_RFPUTMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_RFPUTMSG (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_DYN_RFPUTMSG_SHIFT)) & (ST25DV_GPO_DYN_RFPUTMSG_MASK)) |
                (reg_value & ~(ST25DV_GPO_DYN_RFPUTMSG_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_RFGETMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_DYN_RFGETMSG_MASK);
  *value = *value >> (ST25DV_GPO_DYN_RFGETMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_RFGETMSG (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_DYN_RFGETMSG_SHIFT)) & (ST25DV_GPO_DYN_RFGETMSG_MASK)) |
                (reg_value & ~(ST25DV_GPO_DYN_RFGETMSG_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_RFWRITE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_DYN_RFWRITE_MASK);
  *value = *value >> (ST25DV_GPO_DYN_RFWRITE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_RFWRITE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_DYN_RFWRITE_SHIFT)) & (ST25DV_GPO_DYN_RFWRITE_MASK)) |
                (reg_value & ~(ST25DV_GPO_DYN_RFWRITE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_ENABLE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_GPO_DYN_ENABLE_MASK);
  *value = *value >> (ST25DV_GPO_DYN_ENABLE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_ENABLE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_GPO_DYN_ENABLE_SHIFT)) & (ST25DV_GPO_DYN_ENABLE_MASK)) |
                (reg_value & ~(ST25DV_GPO_DYN_ENABLE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetGPO_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_GPO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_SetGPO_DYN_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  if( ST25DV_WriteReg(ctx, (ST25DV_GPO_DYN_REG), value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetITTIME_DELAY (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITTIME_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITTIME_DELAY_MASK);
  *value = *value >> (ST25DV_ITTIME_DELAY_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetITTIME_DELAY (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_ITTIME_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_ITTIME_DELAY_SHIFT)) & (ST25DV_ITTIME_DELAY_MASK)) |
                (reg_value & ~(ST25DV_ITTIME_DELAY_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_ITTIME_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_RFUSERSTATE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITSTS_DYN_RFUSERSTATE_MASK);
  *value = *value >> (ST25DV_ITSTS_DYN_RFUSERSTATE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_RFACTIVITY (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITSTS_DYN_RFACTIVITY_MASK);
  *value = *value >> (ST25DV_ITSTS_DYN_RFACTIVITY_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_RFINTERRUPT (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITSTS_DYN_RFINTERRUPT_MASK);
  *value = *value >> (ST25DV_ITSTS_DYN_RFINTERRUPT_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_FIELDFALLING (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITSTS_DYN_FIELDFALLING_MASK);
  *value = *value >> (ST25DV_ITSTS_DYN_FIELDFALLING_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_FIELDRISING (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITSTS_DYN_FIELDRISING_MASK);
  *value = *value >> (ST25DV_ITSTS_DYN_FIELDRISING_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_RFPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITSTS_DYN_RFPUTMSG_MASK);
  *value = *value >> (ST25DV_ITSTS_DYN_RFPUTMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_RFGETMSG (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITSTS_DYN_RFGETMSG_MASK);
  *value = *value >> (ST25DV_ITSTS_DYN_RFGETMSG_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_RFWRITE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_ITSTS_DYN_RFWRITE_MASK);
  *value = *value >> (ST25DV_ITSTS_DYN_RFWRITE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetITSTS_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_ITSTS_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetEH_MODE (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_EH_MODE_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_EH_MODE_MASK);
  *value = *value >> (ST25DV_EH_MODE_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetEH_MODE (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_EH_MODE_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_EH_MODE_SHIFT)) & (ST25DV_EH_MODE_MASK)) |
                (reg_value & ~(ST25DV_EH_MODE_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_EH_MODE_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetEH_CTRL_DYN_EH_EN (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_EH_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_EH_CTRL_DYN_EH_EN_MASK);
  *value = *value >> (ST25DV_EH_CTRL_DYN_EH_EN_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetEH_CTRL_DYN_EH_EN (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_EH_CTRL_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_EH_CTRL_DYN_EH_EN_SHIFT)) & (ST25DV_EH_CTRL_DYN_EH_EN_MASK)) |
                (reg_value & ~(ST25DV_EH_CTRL_DYN_EH_EN_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_EH_CTRL_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetEH_CTRL_DYN_EH_ON (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_EH_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_EH_CTRL_DYN_EH_ON_MASK);
  *value = *value >> (ST25DV_EH_CTRL_DYN_EH_ON_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetEH_CTRL_DYN_FIELD_ON (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_EH_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_EH_CTRL_DYN_FIELD_ON_MASK);
  *value = *value >> (ST25DV_EH_CTRL_DYN_FIELD_ON_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetEH_CTRL_DYN_VCC_ON (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_EH_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_EH_CTRL_DYN_VCC_ON_MASK);
  *value = *value >> (ST25DV_EH_CTRL_DYN_VCC_ON_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetEH_CTRL_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_EH_CTRL_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_EH_CTRL_DYN_ALL_MASK);
  *value = *value >> (ST25DV_EH_CTRL_DYN_ALL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_GetRF_MNGT_RFDIS (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RF_MNGT_RFDIS_MASK);
  *value = *value >> (ST25DV_RF_MNGT_RFDIS_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRF_MNGT_RFDIS (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RF_MNGT_RFDIS_SHIFT)) & (ST25DV_RF_MNGT_RFDIS_MASK)) |
                (reg_value & ~(ST25DV_RF_MNGT_RFDIS_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RF_MNGT_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRF_MNGT_RFSLEEP (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RF_MNGT_RFSLEEP_MASK);
  *value = *value >> (ST25DV_RF_MNGT_RFSLEEP_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRF_MNGT_RFSLEEP (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RF_MNGT_RFSLEEP_SHIFT)) & (ST25DV_RF_MNGT_RFSLEEP_MASK)) |
                (reg_value & ~(ST25DV_RF_MNGT_RFSLEEP_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RF_MNGT_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRF_MNGT_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RF_MNGT_ALL_MASK);
  *value = *value >> (ST25DV_RF_MNGT_ALL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRF_MNGT_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RF_MNGT_ALL_SHIFT)) & (ST25DV_RF_MNGT_ALL_MASK)) |
                (reg_value & ~(ST25DV_RF_MNGT_ALL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RF_MNGT_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRF_MNGT_DYN_RFDIS (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RF_MNGT_DYN_RFDIS_MASK);
  *value = *value >> (ST25DV_RF_MNGT_DYN_RFDIS_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRF_MNGT_DYN_RFDIS (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RF_MNGT_DYN_RFDIS_SHIFT)) & (ST25DV_RF_MNGT_DYN_RFDIS_MASK)) |
                (reg_value & ~(ST25DV_RF_MNGT_DYN_RFDIS_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RF_MNGT_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRF_MNGT_DYN_RFSLEEP (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RF_MNGT_DYN_RFSLEEP_MASK);
  *value = *value >> (ST25DV_RF_MNGT_DYN_RFSLEEP_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRF_MNGT_DYN_RFSLEEP (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RF_MNGT_DYN_RFSLEEP_SHIFT)) & (ST25DV_RF_MNGT_DYN_RFSLEEP_MASK)) |
                (reg_value & ~(ST25DV_RF_MNGT_DYN_RFSLEEP_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RF_MNGT_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRF_MNGT_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RF_MNGT_DYN_ALL_MASK);
  *value = *value >> (ST25DV_RF_MNGT_DYN_ALL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRF_MNGT_DYN_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RF_MNGT_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RF_MNGT_DYN_ALL_SHIFT)) & (ST25DV_RF_MNGT_DYN_ALL_MASK)) |
                (reg_value & ~(ST25DV_RF_MNGT_DYN_ALL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RF_MNGT_DYN_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA1SS_PWDCTRL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA1SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA1SS_PWDCTRL_MASK);
  *value = *value >> (ST25DV_RFA1SS_PWDCTRL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA1SS_PWDCTRL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA1SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA1SS_PWDCTRL_SHIFT)) & (ST25DV_RFA1SS_PWDCTRL_MASK)) |
                (reg_value & ~(ST25DV_RFA1SS_PWDCTRL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA1SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA1SS_RWPROT (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA1SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA1SS_RWPROT_MASK);
  *value = *value >> (ST25DV_RFA1SS_RWPROT_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA1SS_RWPROT (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA1SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA1SS_RWPROT_SHIFT)) & (ST25DV_RFA1SS_RWPROT_MASK)) |
                (reg_value & ~(ST25DV_RFA1SS_RWPROT_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA1SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA1SS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA1SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA1SS_ALL_MASK);
  *value = *value >> (ST25DV_RFA1SS_ALL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA1SS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA1SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA1SS_ALL_SHIFT)) & (ST25DV_RFA1SS_ALL_MASK)) |
                (reg_value & ~(ST25DV_RFA1SS_ALL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA1SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA2SS_PWDCTRL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA2SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA2SS_PWDCTRL_MASK);
  *value = *value >> (ST25DV_RFA2SS_PWDCTRL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA2SS_PWDCTRL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA2SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA2SS_PWDCTRL_SHIFT)) & (ST25DV_RFA2SS_PWDCTRL_MASK)) |
                (reg_value & ~(ST25DV_RFA2SS_PWDCTRL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA2SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA2SS_RWPROT (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA2SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA2SS_RWPROT_MASK);
  *value = *value >> (ST25DV_RFA2SS_RWPROT_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA2SS_RWPROT (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA2SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA2SS_RWPROT_SHIFT)) & (ST25DV_RFA2SS_RWPROT_MASK)) |
                (reg_value & ~(ST25DV_RFA2SS_RWPROT_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA2SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA2SS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA2SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA2SS_ALL_MASK);
  *value = *value >> (ST25DV_RFA2SS_ALL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA2SS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA2SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA2SS_ALL_SHIFT)) & (ST25DV_RFA2SS_ALL_MASK)) |
                (reg_value & ~(ST25DV_RFA2SS_ALL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA2SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA3SS_PWDCTRL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA3SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA3SS_PWDCTRL_MASK);
  *value = *value >> (ST25DV_RFA3SS_PWDCTRL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA3SS_PWDCTRL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA3SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA3SS_PWDCTRL_SHIFT)) & (ST25DV_RFA3SS_PWDCTRL_MASK)) |
                (reg_value & ~(ST25DV_RFA3SS_PWDCTRL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA3SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA3SS_RWPROT (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA3SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA3SS_RWPROT_MASK);
  *value = *value >> (ST25DV_RFA3SS_RWPROT_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA3SS_RWPROT (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA3SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA3SS_RWPROT_SHIFT)) & (ST25DV_RFA3SS_RWPROT_MASK)) |
                (reg_value & ~(ST25DV_RFA3SS_RWPROT_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA3SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA3SS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA3SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA3SS_ALL_MASK);
  *value = *value >> (ST25DV_RFA3SS_ALL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA3SS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA3SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA3SS_ALL_SHIFT)) & (ST25DV_RFA3SS_ALL_MASK)) |
                (reg_value & ~(ST25DV_RFA3SS_ALL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA3SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA4SS_PWDCTRL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA4SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA4SS_PWDCTRL_MASK);
  *value = *value >> (ST25DV_RFA4SS_PWDCTRL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA4SS_PWDCTRL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA4SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA4SS_PWDCTRL_SHIFT)) & (ST25DV_RFA4SS_PWDCTRL_MASK)) |
                (reg_value & ~(ST25DV_RFA4SS_PWDCTRL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA4SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA4SS_RWPROT (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA4SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA4SS_RWPROT_MASK);
  *value = *value >> (ST25DV_RFA4SS_RWPROT_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA4SS_RWPROT (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA4SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA4SS_RWPROT_SHIFT)) & (ST25DV_RFA4SS_RWPROT_MASK)) |
                (reg_value & ~(ST25DV_RFA4SS_RWPROT_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA4SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetRFA4SS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA4SS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_RFA4SS_ALL_MASK);
  *value = *value >> (ST25DV_RFA4SS_ALL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetRFA4SS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_RFA4SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_RFA4SS_ALL_SHIFT)) & (ST25DV_RFA4SS_ALL_MASK)) |
                (reg_value & ~(ST25DV_RFA4SS_ALL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_RFA4SS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetI2CSS_PZ1 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_I2CSS_PZ1_MASK);
  *value = *value >> (ST25DV_I2CSS_PZ1_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetI2CSS_PZ1 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_I2CSS_PZ1_SHIFT)) & (ST25DV_I2CSS_PZ1_MASK)) |
                (reg_value & ~(ST25DV_I2CSS_PZ1_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_I2CSS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetI2CSS_PZ2 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_I2CSS_PZ2_MASK);
  *value = *value >> (ST25DV_I2CSS_PZ2_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetI2CSS_PZ2 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_I2CSS_PZ2_SHIFT)) & (ST25DV_I2CSS_PZ2_MASK)) |
                (reg_value & ~(ST25DV_I2CSS_PZ2_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_I2CSS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetI2CSS_PZ3 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_I2CSS_PZ3_MASK);
  *value = *value >> (ST25DV_I2CSS_PZ3_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetI2CSS_PZ3 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_I2CSS_PZ3_SHIFT)) & (ST25DV_I2CSS_PZ3_MASK)) |
                (reg_value & ~(ST25DV_I2CSS_PZ3_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_I2CSS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetI2CSS_PZ4 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_I2CSS_PZ4_MASK);
  *value = *value >> (ST25DV_I2CSS_PZ4_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetI2CSS_PZ4 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_I2CSS_PZ4_SHIFT)) & (ST25DV_I2CSS_PZ4_MASK)) |
                (reg_value & ~(ST25DV_I2CSS_PZ4_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_I2CSS_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetI2CSS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_I2CSS_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_SetI2CSS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  if( ST25DV_WriteReg(ctx, (ST25DV_I2CSS_REG), value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetLOCKCCFILE_BLCK0 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKCCFILE_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_LOCKCCFILE_BLCK0_MASK);
  *value = *value >> (ST25DV_LOCKCCFILE_BLCK0_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetLOCKCCFILE_BLCK0 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKCCFILE_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_LOCKCCFILE_BLCK0_SHIFT)) & (ST25DV_LOCKCCFILE_BLCK0_MASK)) |
                (reg_value & ~(ST25DV_LOCKCCFILE_BLCK0_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_LOCKCCFILE_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetLOCKCCFILE_BLCK1 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKCCFILE_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_LOCKCCFILE_BLCK1_MASK);
  *value = *value >> (ST25DV_LOCKCCFILE_BLCK1_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetLOCKCCFILE_BLCK1 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKCCFILE_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_LOCKCCFILE_BLCK1_SHIFT)) & (ST25DV_LOCKCCFILE_BLCK1_MASK)) |
                (reg_value & ~(ST25DV_LOCKCCFILE_BLCK1_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_LOCKCCFILE_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetLOCKCCFILE_ALL (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKCCFILE_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_LOCKCCFILE_ALL_MASK);
  *value = *value >> (ST25DV_LOCKCCFILE_ALL_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetLOCKCCFILE_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKCCFILE_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_LOCKCCFILE_ALL_SHIFT)) & (ST25DV_LOCKCCFILE_ALL_MASK)) |
                (reg_value & ~(ST25DV_LOCKCCFILE_ALL_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_LOCKCCFILE_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetLOCKCFG_B0 (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKCFG_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_LOCKCFG_B0_MASK);
  *value = *value >> (ST25DV_LOCKCFG_B0_SHIFT);
  return NFCTAG_OK;
}

int32_t ST25DV_SetLOCKCFG_B0 (ST25DV_Ctx_t *ctx, const uint8_t *value)
{
  uint8_t reg_value;
  if( ST25DV_ReadReg(ctx, (ST25DV_LOCKCFG_REG), &reg_value, 1))
    return NFCTAG_ERROR;

  reg_value = ( (*value << (ST25DV_LOCKCFG_B0_SHIFT)) & (ST25DV_LOCKCFG_B0_MASK)) |
                (reg_value & ~(ST25DV_LOCKCFG_B0_MASK));

  if( ST25DV_WriteReg(ctx, (ST25DV_LOCKCFG_REG), &reg_value, 1))
    return NFCTAG_ERROR;
  return NFCTAG_OK;
}

int32_t ST25DV_GetI2C_SSO_DYN_I2CSSO (ST25DV_Ctx_t *ctx, uint8_t *value)
{
  if( ST25DV_ReadReg(ctx, (ST25DV_I2C_SSO_DYN_REG), (uint8_t *)value, 1))
    return NFCTAG_ERROR;
  *value &= (ST25DV_I2C_SSO_DYN_I2CSSO_MASK);
  *value = *value >> (ST25DV_I2C_SSO_DYN_I2CSSO_SHIFT);
  return NFCTAG_OK;
}



