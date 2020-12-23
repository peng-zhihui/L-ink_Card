#include "main.h"
#include "app_nfc.h"
#include "nfc04a1_nfctag.h"
#include "../E-Paper-Display/epd_w21.h"
#include <stdio.h>

uint8_t cnt = 0;
uint16_t mblength;
ST25DV_MB_CTRL_DYN_STATUS mbctrldynstatus;
ST25DV_EN_STATUS MB_mode;
ST25DV_PASSWD passwd;
ST25DV_I2CSSO_STATUS i2csso;
volatile uint8_t GPOActivated = 0;

void MX_NFC4_MAILBOX_Init(void);

void MX_NFC4_MAILBOX_Process(void);

void MX_NFC_Init(void)
{
    MX_NFC4_MAILBOX_Init();
}

void MX_NFC_Process(void)
{
    MX_NFC4_MAILBOX_Process();
}


void MX_NFC4_MAILBOX_Init(void)
{
    /* Init ST25DV driver */
    while (NFC04A1_NFCTAG_Init(NFC04A1_NFCTAG_INSTANCE) != NFCTAG_OK);

    /* You need to present password to change static configuration */
    NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(NFC04A1_NFCTAG_INSTANCE, &i2csso);
    if (i2csso == ST25DV_SESSION_CLOSED)
    {
        /* if I2C session is closed, present password to open session */
        passwd.MsbPasswd = 0;
        passwd.LsbPasswd = 0;
        NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
    }

    /* Energy harvesting activated after Power On Reset */
    NFC04A1_NFCTAG_WriteEHMode(NFC04A1_NFCTAG_INSTANCE, ST25DV_EH_ACTIVE_AFTER_BOOT);
    NFC04A1_NFCTAG_SetEHENMode_Dyn(NFC04A1_NFCTAG_INSTANCE);

    /* If not activated, activate Mailbox, as long as MB is ON EEPROM is not available */
    NFC04A1_NFCTAG_ReadMBMode(NFC04A1_NFCTAG_INSTANCE, &MB_mode);
    if (MB_mode == ST25DV_DISABLE)
    {
        /* You need to present password before changing static configuration */
        NFC04A1_NFCTAG_ReadI2CSecuritySession_Dyn(NFC04A1_NFCTAG_INSTANCE, &i2csso);
        if (i2csso == ST25DV_SESSION_CLOSED)
        {
            /* if I2C session is closed, present password to open session */
            passwd.MsbPasswd = 0; /* Default value for password */
            passwd.LsbPasswd = 0; /* change it if password has been modified */
            NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
        }
        NFC04A1_NFCTAG_WriteMBMode(NFC04A1_NFCTAG_INSTANCE, ST25DV_ENABLE);

        /* Close session as dynamic register doesn't need open session for modification */
        passwd.MsbPasswd = 123;
        passwd.LsbPasswd = 123;
        NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
    }

    /* Enable Mailbox in dynamique register */
    NFC04A1_NFCTAG_SetMBEN_Dyn(NFC04A1_NFCTAG_INSTANCE);
    printf("\n\r\n\rMailbox is activated");

    /* Set EXTI settings for GPO Interrupt */
    NFC04A1_GPO_Init();
    /* Present configuration password */
    passwd.MsbPasswd = 0; /* Default value for password */
    passwd.LsbPasswd = 0; /* change it if password has been modified */
    NFC04A1_NFCTAG_PresentI2CPassword(NFC04A1_NFCTAG_INSTANCE, passwd);
    /* Set GPO Configuration */
    NFC04A1_NFCTAG_ConfigIT(NFC04A1_NFCTAG_INSTANCE,
                            ST25DV_GPO_ENABLE_MASK | ST25DV_GPO_RFGETMSG_MASK | ST25DV_GPO_RFPUTMSG_MASK);

}

/**
* @brief  Process of the MAILBOX application
* @retval None
*/

extern unsigned char nfcBuffer[];
int bufferIndex = 0;

void MX_NFC4_MAILBOX_Process(void)
{
    if (GPOActivated == 1)
    {
        /* Check if Mailbox is available */
        NFC04A1_NFCTAG_ReadMBCtrl_Dyn(NFC04A1_NFCTAG_INSTANCE, &mbctrldynstatus);

        if (mbctrldynstatus.RfPutMsg == 1)
        {
            /* Read length of message */
            NFC04A1_NFCTAG_ReadMBLength_Dyn(NFC04A1_NFCTAG_INSTANCE, (uint8_t *) &mblength);
            mblength += 1;

#if DEBUG
            /* Read mailbox status */
            NFC04A1_NFCTAG_ReadMBCtrl_Dyn(NFC04A1_NFCTAG_INSTANCE, &mbctrldynstatus);
            printf("\n\r\n\rCtrl MB status register value:");
            printf("\n\rHost(i2c) Missed Message  = %d", mbctrldynstatus.HostMissMsg);
            printf("\n\rRF(reader) Missed Message = %d", mbctrldynstatus.RFMissMsg);
            printf("\n\rHost(i2c) Put Message     = %d", mbctrldynstatus.HostPutMsg);
            printf("\n\rRF(reader) Put Message    = %d", mbctrldynstatus.RfPutMsg);
            printf("\n\rMailbox Enable            = %d", mbctrldynstatus.MbEnable);
#endif

            if (mblength == 3) // frame header
            {
                bufferIndex = 0;
            } else if (mblength == 200) // picture data
            {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

                /* Read all data in Mailbox */
                NFC04A1_NFCTAG_ReadMailboxData(NFC04A1_NFCTAG_INSTANCE, nfcBuffer + bufferIndex, 0, 200);
                bufferIndex += 200;

                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
            }

            if (bufferIndex == 5000)
            {
                bufferIndex = 0;

                EpdDisFull((unsigned char *) nfcBuffer, 1);
                HAL_Delay(3000);
            }

        }

        GPOActivated = 0;
    }
}


void BSP_GPO_Callback(void)
{
    /* Prevent unused argument(s) compilation warning */
    GPOActivated = 1;
    /* This function should be implemented by the user application.
       It is called into this driver when an event on Button is triggered. */
}


