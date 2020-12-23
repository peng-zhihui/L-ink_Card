#ifndef _ST25DV_REG_H_
#define _ST25DV_REG_H_

#include <stdint.h>

/* Registers i2c address */
/** @brief ST25DV GPO register address. */
#define ST25DV_GPO_REG                       0x0000
/** @brief ST25DV IT duration register address. */
#define ST25DV_ITTIME_REG                    0x0001
/** @brief ST25DV Energy Harvesting register address. */
#define ST25DV_EH_MODE_REG                   0x0002
/** @brief ST25DV RF management register address. */
#define ST25DV_RF_MNGT_REG                   0x0003
/** @brief ST25DV Area 1 security register address. */
#define ST25DV_RFA1SS_REG                    0x0004
/** @brief ST25DV Area 1 end address register address. */
#define ST25DV_ENDA1_REG                      0x0005
/** @brief ST25DV Area 2 security register address. */
#define ST25DV_RFA2SS_REG                    0x0006
/** @brief ST25DV Area 2 end address register address. */
#define ST25DV_ENDA2_REG                      0x0007
/** @brief ST25DV Area 3 security register address. */
#define ST25DV_RFA3SS_REG                    0x0008
/** @brief ST25DV Area 3 end address register address. */
#define ST25DV_ENDA3_REG                      0x0009
/** @brief ST25DV Area 4 security register address. */
#define ST25DV_RFA4SS_REG                    0x000A
/** @brief ST25DV I2C security register address. */
#define ST25DV_I2CSS_REG                    0x000B
/** @brief ST25DV Capability Container lock register address. */
#define ST25DV_LOCKCCFILE_REG                0x000C
/** @brief ST25DV Mailbox mode register address. */
#define ST25DV_MB_MODE_REG                   0x000D
/** @brief ST25DV Mailbox Watchdog register address. */
#define ST25DV_MB_WDG_REG                    0x000E
/** @brief ST25DV Configuration lock register address. */
#define ST25DV_LOCKCFG_REG                   0x000F
/** @brief ST25DV DSFID lock register address. */
#define ST25DV_LOCKDSFID_REG                 0x0010
/** @brief ST25DV AFI lock register address. */
#define ST25DV_LOCKAFI_REG                   0x0011
/** @brief ST25DV DSFID register address. */
#define ST25DV_DSFID_REG                     0x0012
/** @brief ST25DV AFI register address. */
#define ST25DV_AFI_REG                       0x0013
/** @brief ST25DV Memory size register address. */
#define ST25DV_MEM_SIZE_LSB_REG              0x0014
/** @brief ST25DV Memory size register address. */
#define ST25DV_MEM_SIZE_MSB_REG              0x0015
/** @brief ST25DV Block size register address. */
#define ST25DV_BLK_SIZE_REG                  0x0016
//** @brief ST25DV ICref register address. */
#define ST25DV_ICREF_REG                     0x0017
/** @brief ST25DV UID register address. */
#define ST25DV_UID_REG                       0x0018
/** @brief ST25DV IC revision register address. */
#define ST25DV_ICREV_REG                     0x0020
/** @brief ST25DV I2C password register address. */
#define ST25DV_I2CPASSWD_REG                 0x0900

/* Dynamic Registers i2c address */
/** @brief ST25DV GPO dynamic register address. */
#define ST25DV_GPO_DYN_REG                   0x2000
/** @brief ST25DV Energy Harvesting control dynamic register address. */
#define ST25DV_EH_CTRL_DYN_REG               0x2002
/** @brief ST25DV RF management dynamic register address. */
#define ST25DV_RF_MNGT_DYN_REG               0x2003
/** @brief ST25DV I2C secure session opened dynamic register address. */
#define ST25DV_I2C_SSO_DYN_REG               0x2004
/** @brief ST25DV Interrupt status dynamic register address. */
#define ST25DV_ITSTS_DYN_REG                 0x2005
/** @brief ST25DV Mailbox control dynamic register address. */
#define ST25DV_MB_CTRL_DYN_REG               0x2006
/** @brief ST25DV Mailbox message length dynamic register address. */
#define ST25DV_MBLEN_DYN_REG                 0x2007
/** @brief ST25DV Mailbox buffer address. */
#define ST25DV_MAILBOX_RAM_REG               0x2008

/* Registers fields definitions */
/* MB_MODE */
#define ST25DV_MB_MODE_RW_SHIFT              (0)
#define ST25DV_MB_MODE_RW_FIELD              0xFE
#define ST25DV_MB_MODE_RW_MASK               0x01

/* MB_LEN_Dyn */
#define ST25DV_MBLEN_DYN_MBLEN_SHIFT         (0)
#define ST25DV_MBLEN_DYN_MBLEN_FIELD         0x00
#define ST25DV_MBLEN_DYN_MBLEN_MASK          0xFF

/* MB_CTRL_Dyn */
#define ST25DV_MB_CTRL_DYN_MBEN_SHIFT        (0)
#define ST25DV_MB_CTRL_DYN_MBEN_FIELD        0xFE
#define ST25DV_MB_CTRL_DYN_MBEN_MASK         0x01
#define ST25DV_MB_CTRL_DYN_HOSTPUTMSG_SHIFT  (1)
#define ST25DV_MB_CTRL_DYN_HOSTPUTMSG_FIELD  0xFD
#define ST25DV_MB_CTRL_DYN_HOSTPUTMSG_MASK   0x02
#define ST25DV_MB_CTRL_DYN_RFPUTMSG_SHIFT    (2)
#define ST25DV_MB_CTRL_DYN_RFPUTMSG_FIELD    0xFB
#define ST25DV_MB_CTRL_DYN_RFPUTMSG_MASK     0x04
#define ST25DV_MB_CTRL_DYN_STRESERVED_SHIFT  (3)
#define ST25DV_MB_CTRL_DYN_STRESERVED_FIELD  0xF7
#define ST25DV_MB_CTRL_DYN_STRESERVED_MASK   0x08
#define ST25DV_MB_CTRL_DYN_HOSTMISSMSG_SHIFT (4)
#define ST25DV_MB_CTRL_DYN_HOSTMISSMSG_FIELD 0xEF
#define ST25DV_MB_CTRL_DYN_HOSTMISSMSG_MASK  0x10
#define ST25DV_MB_CTRL_DYN_RFMISSMSG_SHIFT   (5)
#define ST25DV_MB_CTRL_DYN_RFMISSMSG_FIELD   0xDF
#define ST25DV_MB_CTRL_DYN_RFMISSMSG_MASK    0x20
#define ST25DV_MB_CTRL_DYN_CURRENTMSG_SHIFT  (6)
#define ST25DV_MB_CTRL_DYN_CURRENTMSG_FIELD  0x3F
#define ST25DV_MB_CTRL_DYN_CURRENTMSG_MASK   0xC0


/* MB_WDG */
#define ST25DV_MB_WDG_DELAY_SHIFT            (0)
#define ST25DV_MB_WDG_DELAY_FIELD            0xF8
#define ST25DV_MB_WDG_DELAY_MASK             0x07

/* GPO */
#define ST25DV_GPO_RFUSERSTATE_SHIFT         (0)
#define ST25DV_GPO_RFUSERSTATE_FIELD         0xFE
#define ST25DV_GPO_RFUSERSTATE_MASK          0x01
#define ST25DV_GPO_RFACTIVITY_SHIFT          (1)
#define ST25DV_GPO_RFACTIVITY_FIELD          0xFD
#define ST25DV_GPO_RFACTIVITY_MASK           0x02
#define ST25DV_GPO_RFINTERRUPT_SHIFT         (2)
#define ST25DV_GPO_RFINTERRUPT_FIELD         0xFB
#define ST25DV_GPO_RFINTERRUPT_MASK          0x04
#define ST25DV_GPO_FIELDCHANGE_SHIFT         (3)
#define ST25DV_GPO_FIELDCHANGE_FIELD         0xF7
#define ST25DV_GPO_FIELDCHANGE_MASK          0x08
#define ST25DV_GPO_RFPUTMSG_SHIFT            (4)
#define ST25DV_GPO_RFPUTMSG_FIELD            0xEF
#define ST25DV_GPO_RFPUTMSG_MASK             0x10
#define ST25DV_GPO_RFGETMSG_SHIFT            (5)
#define ST25DV_GPO_RFGETMSG_FIELD            0xDF
#define ST25DV_GPO_RFGETMSG_MASK             0x20
#define ST25DV_GPO_RFWRITE_SHIFT             (6)
#define ST25DV_GPO_RFWRITE_FIELD             0xBF
#define ST25DV_GPO_RFWRITE_MASK              0x40
#define ST25DV_GPO_ENABLE_SHIFT              (7)
#define ST25DV_GPO_ENABLE_FIELD              0x7F
#define ST25DV_GPO_ENABLE_MASK               0x80
#define ST25DV_GPO_ALL_SHIFT                 (0)
#define ST25DV_GPO_ALL_MASK                  0xFF

/* GPO_Dyn */
#define ST25DV_GPO_DYN_RFUSERSTATE_SHIFT     (0)
#define ST25DV_GPO_DYN_RFUSERSTATE_FIELD     0xFE
#define ST25DV_GPO_DYN_RFUSERSTATE_MASK      0x01
#define ST25DV_GPO_DYN_RFACTIVITY_SHIFT      (1)
#define ST25DV_GPO_DYN_RFACTIVITY_FIELD      0xFD
#define ST25DV_GPO_DYN_RFACTIVITY_MASK       0x02
#define ST25DV_GPO_DYN_RFINTERRUPT_SHIFT     (2)
#define ST25DV_GPO_DYN_RFINTERRUPT_FIELD     0xFB
#define ST25DV_GPO_DYN_RFINTERRUPT_MASK      0x04
#define ST25DV_GPO_DYN_FIELDCHANGE_SHIFT     (3)
#define ST25DV_GPO_DYN_FIELDCHANGE_FIELD     0xF7
#define ST25DV_GPO_DYN_FIELDCHANGE_MASK      0x08
#define ST25DV_GPO_DYN_RFPUTMSG_SHIFT        (4)
#define ST25DV_GPO_DYN_RFPUTMSG_FIELD        0xEF
#define ST25DV_GPO_DYN_RFPUTMSG_MASK         0x10
#define ST25DV_GPO_DYN_RFGETMSG_SHIFT        (5)
#define ST25DV_GPO_DYN_RFGETMSG_FIELD        0xDF
#define ST25DV_GPO_DYN_RFGETMSG_MASK         0x20
#define ST25DV_GPO_DYN_RFWRITE_SHIFT         (6)
#define ST25DV_GPO_DYN_RFWRITE_FIELD         0xBF
#define ST25DV_GPO_DYN_RFWRITE_MASK          0x40
#define ST25DV_GPO_DYN_ENABLE_SHIFT          (7)
#define ST25DV_GPO_DYN_ENABLE_FIELD          0x7F
#define ST25DV_GPO_DYN_ENABLE_MASK           0x80
#define ST25DV_GPO_DYN_ALL_SHIFT             (0)
#define ST25DV_GPO_DYN_ALL_MASK              0xFF

/* ITTIME */
#define ST25DV_ITTIME_DELAY_SHIFT            (0)
#define ST25DV_ITTIME_DELAY_FIELD            0xFC
#define ST25DV_ITTIME_DELAY_MASK             0x03

/* ITSTS_Dyn */
#define ST25DV_ITSTS_DYN_RFUSERSTATE_SHIFT       (0)
#define ST25DV_ITSTS_DYN_RFUSERSTATE_FIELD       0xFE
#define ST25DV_ITSTS_DYN_RFUSERSTATE_MASK        0x01
#define ST25DV_ITSTS_DYN_RFACTIVITY_SHIFT        (1)
#define ST25DV_ITSTS_DYN_RFACTIVITY_FIELD        0xFD
#define ST25DV_ITSTS_DYN_RFACTIVITY_MASK         0x02
#define ST25DV_ITSTS_DYN_RFINTERRUPT_SHIFT       (2)
#define ST25DV_ITSTS_DYN_RFINTERRUPT_FIELD       0xFB
#define ST25DV_ITSTS_DYN_RFINTERRUPT_MASK        0x04
#define ST25DV_ITSTS_DYN_FIELDFALLING_SHIFT      (3)
#define ST25DV_ITSTS_DYN_FIELDFALLING_FIELD      0xF7
#define ST25DV_ITSTS_DYN_FIELDFALLING_MASK       0x08
#define ST25DV_ITSTS_DYN_FIELDRISING_SHIFT       (4)
#define ST25DV_ITSTS_DYN_FIELDRISING_FIELD       0xEF
#define ST25DV_ITSTS_DYN_FIELDRISING_MASK        0x10
#define ST25DV_ITSTS_DYN_RFPUTMSG_SHIFT          (5)
#define ST25DV_ITSTS_DYN_RFPUTMSG_FIELD          0xDF
#define ST25DV_ITSTS_DYN_RFPUTMSG_MASK           0x20
#define ST25DV_ITSTS_DYN_RFGETMSG_SHIFT          (6)
#define ST25DV_ITSTS_DYN_RFGETMSG_FIELD          0xBF
#define ST25DV_ITSTS_DYN_RFGETMSG_MASK           0x40
#define ST25DV_ITSTS_DYN_RFWRITE_SHIFT           (7)
#define ST25DV_ITSTS_DYN_RFWRITE_FIELD           0x7F
#define ST25DV_ITSTS_DYN_RFWRITE_MASK            0x80

/* EH_MODE */
#define ST25DV_EH_MODE_SHIFT                 (0)
#define ST25DV_EH_MODE_FIELD                 0xFE
#define ST25DV_EH_MODE_MASK                  0x01

/* EH_CTRL_Dyn */
#define ST25DV_EH_CTRL_DYN_EH_EN_SHIFT       (0)
#define ST25DV_EH_CTRL_DYN_EH_EN_FIELD       0xFE
#define ST25DV_EH_CTRL_DYN_EH_EN_MASK        0x01
#define ST25DV_EH_CTRL_DYN_EH_ON_SHIFT       (1)
#define ST25DV_EH_CTRL_DYN_EH_ON_FIELD       0xFD
#define ST25DV_EH_CTRL_DYN_EH_ON_MASK        0x02
#define ST25DV_EH_CTRL_DYN_FIELD_ON_SHIFT    (2)
#define ST25DV_EH_CTRL_DYN_FIELD_ON_FIELD    0xFB
#define ST25DV_EH_CTRL_DYN_FIELD_ON_MASK     0x04
#define ST25DV_EH_CTRL_DYN_VCC_ON_SHIFT      (3)
#define ST25DV_EH_CTRL_DYN_VCC_ON_FIELD      0xF7
#define ST25DV_EH_CTRL_DYN_VCC_ON_MASK       0x08
#define ST25DV_EH_CTRL_DYN_ALL_SHIFT         (0)
#define ST25DV_EH_CTRL_DYN_ALL_MASK          0x0F

/* RF_MNGT */
#define ST25DV_RF_MNGT_RFDIS_SHIFT           (0)
#define ST25DV_RF_MNGT_RFDIS_FIELD           0xFE
#define ST25DV_RF_MNGT_RFDIS_MASK            0x01
#define ST25DV_RF_MNGT_RFSLEEP_SHIFT         (1)
#define ST25DV_RF_MNGT_RFSLEEP_FIELD         0xFD
#define ST25DV_RF_MNGT_RFSLEEP_MASK          0x02
#define ST25DV_RF_MNGT_ALL_SHIFT             (0)
#define ST25DV_RF_MNGT_ALL_MASK              0x03

/* RF_MNGT_Dyn */
#define ST25DV_RF_MNGT_DYN_RFDIS_SHIFT       (0)
#define ST25DV_RF_MNGT_DYN_RFDIS_FIELD       0xFE
#define ST25DV_RF_MNGT_DYN_RFDIS_MASK        0x01
#define ST25DV_RF_MNGT_DYN_RFSLEEP_SHIFT     (1)
#define ST25DV_RF_MNGT_DYN_RFSLEEP_FIELD     0xFD
#define ST25DV_RF_MNGT_DYN_RFSLEEP_MASK      0x02
#define ST25DV_RF_MNGT_DYN_ALL_SHIFT         (0)
#define ST25DV_RF_MNGT_DYN_ALL_MASK          0x03

/* RFA1SS */
#define ST25DV_RFA1SS_PWDCTRL_SHIFT           (0)
#define ST25DV_RFA1SS_PWDCTRL_FIELD           0xFC
#define ST25DV_RFA1SS_PWDCTRL_MASK            0x03
#define ST25DV_RFA1SS_RWPROT_SHIFT            (2)
#define ST25DV_RFA1SS_RWPROT_FIELD            0xF3
#define ST25DV_RFA1SS_RWPROT_MASK             0x0C
#define ST25DV_RFA1SS_ALL_SHIFT               (0)
#define ST25DV_RFA1SS_ALL_MASK                0x0F

/* RFA2SS */
#define ST25DV_RFA2SS_PWDCTRL_SHIFT           (0)
#define ST25DV_RFA2SS_PWDCTRL_FIELD           0xFC
#define ST25DV_RFA2SS_PWDCTRL_MASK            0x03
#define ST25DV_RFA2SS_RWPROT_SHIFT            (2)
#define ST25DV_RFA2SS_RWPROT_FIELD            0xF3
#define ST25DV_RFA2SS_RWPROT_MASK             0x0C
#define ST25DV_RFA2SS_ALL_SHIFT               (0)
#define ST25DV_RFA2SS_ALL_MASK                0x0F

/* RFA3SS */
#define ST25DV_RFA3SS_PWDCTRL_SHIFT           (0)
#define ST25DV_RFA3SS_PWDCTRL_FIELD           0xFC
#define ST25DV_RFA3SS_PWDCTRL_MASK            0x03
#define ST25DV_RFA3SS_RWPROT_SHIFT            (2)
#define ST25DV_RFA3SS_RWPROT_FIELD            0xF3
#define ST25DV_RFA3SS_RWPROT_MASK             0x0C
#define ST25DV_RFA3SS_ALL_SHIFT               (0)
#define ST25DV_RFA3SS_ALL_MASK                0x0F

/* RFA4SS */
#define ST25DV_RFA4SS_PWDCTRL_SHIFT           (0)
#define ST25DV_RFA4SS_PWDCTRL_FIELD           0xFC
#define ST25DV_RFA4SS_PWDCTRL_MASK            0x03
#define ST25DV_RFA4SS_RWPROT_SHIFT            (2)
#define ST25DV_RFA4SS_RWPROT_FIELD            0xF3
#define ST25DV_RFA4SS_RWPROT_MASK             0x0C
#define ST25DV_RFA4SS_ALL_SHIFT               (0)
#define ST25DV_RFA4SS_ALL_MASK                0x0F

/* I2CSS */
#define ST25DV_I2CSS_PZ1_SHIFT              (0)
#define ST25DV_I2CSS_PZ1_FIELD              0xFC
#define ST25DV_I2CSS_PZ1_MASK               0x03
#define ST25DV_I2CSS_PZ2_SHIFT              (2)
#define ST25DV_I2CSS_PZ2_FIELD              0xF3
#define ST25DV_I2CSS_PZ2_MASK               0x0C
#define ST25DV_I2CSS_PZ3_SHIFT              (4)
#define ST25DV_I2CSS_PZ3_FIELD              0xCF
#define ST25DV_I2CSS_PZ3_MASK               0x30
#define ST25DV_I2CSS_PZ4_SHIFT              (6)
#define ST25DV_I2CSS_PZ4_FIELD              0x3F
#define ST25DV_I2CSS_PZ4_MASK               0xC0

/* LOCKCCFILE */
#define ST25DV_LOCKCCFILE_BLCK0_SHIFT        (0)
#define ST25DV_LOCKCCFILE_BLCK0_FIELD        0xFE
#define ST25DV_LOCKCCFILE_BLCK0_MASK         0x01
#define ST25DV_LOCKCCFILE_BLCK1_SHIFT        (1)
#define ST25DV_LOCKCCFILE_BLCK1_FIELD        0xFD
#define ST25DV_LOCKCCFILE_BLCK1_MASK         0x02
#define ST25DV_LOCKCCFILE_ALL_SHIFT          (0)
#define ST25DV_LOCKCCFILE_ALL_MASK           0x03

/* LOCKCFG */
#define ST25DV_LOCKCFG_B0_SHIFT              (0)
#define ST25DV_LOCKCFG_B0_FIELD              0xFE
#define ST25DV_LOCKCFG_B0_MASK               0x01

/* I2C_SSO_Dyn */
#define ST25DV_I2C_SSO_DYN_I2CSSO_SHIFT      (0)
#define ST25DV_I2C_SSO_DYN_I2CSSO_FIELD      0xFE
#define ST25DV_I2C_SSO_DYN_I2CSSO_MASK       0x01

/**
 * @brief  NFCTAG status enumerator definition.
 */
#define NFCTAG_OK      (0)
#define NFCTAG_ERROR   (-1)
#define NFCTAG_BUSY    (-2)
#define NFCTAG_TIMEOUT (-3)
#define NFCTAG_NACK    (-102)


typedef int32_t (*ST25DV_WriteReg_Func)(void *, uint16_t, const uint8_t*, uint16_t);
typedef int32_t (*ST25DV_ReadReg_Func) (void *, uint16_t, uint8_t*, uint16_t);

typedef struct {
  ST25DV_WriteReg_Func WriteReg;
  ST25DV_ReadReg_Func ReadReg;
  void *handle;
} ST25DV_Ctx_t;

int32_t ST25DV_GetICREF (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetENDA1 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetENDA1 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetENDA2 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetENDA2 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetENDA3 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetENDA3 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetDSFID (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetAFI (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMEM_SIZE_MSB (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetBLK_SIZE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMEM_SIZE_LSB (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetICREV (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetUID (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetI2CPASSWD (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetI2CPASSWD (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetLOCKDSFID (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetLOCKAFI (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_MODE_RW (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetMB_MODE_RW (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetMBLEN_DYN_MBLEN (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_CTRL_DYN_MBEN (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetMB_CTRL_DYN_MBEN (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetMB_CTRL_DYN_HOSTPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_CTRL_DYN_RFPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_CTRL_DYN_STRESERVED (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_CTRL_DYN_HOSTMISSMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_CTRL_DYN_RFMISSMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_CTRL_DYN_CURRENTMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_CTRL_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetMB_WDG_DELAY (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetMB_WDG_DELAY (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_RFUSERSTATE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_RFUSERSTATE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_RFACTIVITY (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_RFACTIVITY (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_RFINTERRUPT (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_RFINTERRUPT (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_FIELDCHANGE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_FIELDCHANGE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_RFPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_RFPUTMSG (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_RFGETMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_RFGETMSG (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_RFWRITE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_RFWRITE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_ENABLE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_ENABLE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_RFUSERSTATE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_RFUSERSTATE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_RFACTIVITY (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_RFACTIVITY (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_RFINTERRUPT (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_RFINTERRUPT (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_FIELDCHANGE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_FIELDCHANGE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_RFPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_RFPUTMSG (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_RFGETMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_RFGETMSG (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_RFWRITE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_RFWRITE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_ENABLE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_ENABLE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetGPO_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetGPO_DYN_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetITTIME_DELAY (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetITTIME_DELAY (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_RFUSERSTATE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_RFACTIVITY (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_RFINTERRUPT (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_FIELDFALLING (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_FIELDRISING (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_RFPUTMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_RFGETMSG (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_RFWRITE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetITSTS_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetEH_MODE (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetEH_MODE (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetEH_CTRL_DYN_EH_EN (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetEH_CTRL_DYN_EH_EN (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetEH_CTRL_DYN_EH_ON (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetEH_CTRL_DYN_FIELD_ON (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetEH_CTRL_DYN_VCC_ON (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetEH_CTRL_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_GetRF_MNGT_RFDIS (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRF_MNGT_RFDIS (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRF_MNGT_RFSLEEP (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRF_MNGT_RFSLEEP (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRF_MNGT_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRF_MNGT_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRF_MNGT_DYN_RFDIS (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRF_MNGT_DYN_RFDIS (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRF_MNGT_DYN_RFSLEEP (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRF_MNGT_DYN_RFSLEEP (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRF_MNGT_DYN_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRF_MNGT_DYN_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA1SS_PWDCTRL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA1SS_PWDCTRL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA1SS_RWPROT (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA1SS_RWPROT (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA1SS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA1SS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA2SS_PWDCTRL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA2SS_PWDCTRL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA2SS_RWPROT (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA2SS_RWPROT (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA2SS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA2SS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA3SS_PWDCTRL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA3SS_PWDCTRL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA3SS_RWPROT (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA3SS_RWPROT (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA3SS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA3SS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA4SS_PWDCTRL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA4SS_PWDCTRL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA4SS_RWPROT (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA4SS_RWPROT (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetRFA4SS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetRFA4SS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetI2CSS_PZ1 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetI2CSS_PZ1 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetI2CSS_PZ2 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetI2CSS_PZ2 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetI2CSS_PZ3 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetI2CSS_PZ3 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetI2CSS_PZ4 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetI2CSS_PZ4 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetI2CSS_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetI2CSS_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetLOCKCCFILE_BLCK0 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetLOCKCCFILE_BLCK0 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetLOCKCCFILE_BLCK1 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetLOCKCCFILE_BLCK1 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetLOCKCCFILE_ALL (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetLOCKCCFILE_ALL (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetLOCKCFG_B0 (ST25DV_Ctx_t *ctx, uint8_t *value);
int32_t ST25DV_SetLOCKCFG_B0 (ST25DV_Ctx_t *ctx, const uint8_t *value);
int32_t ST25DV_GetI2C_SSO_DYN_I2CSSO (ST25DV_Ctx_t *ctx, uint8_t *value);

#endif
