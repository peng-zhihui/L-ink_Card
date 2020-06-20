#include "Display_EPD_W21_Config.h"
#include "stm32l0xx_hal.h"

void SPI_Delay(unsigned char xrate)
{
    unsigned char i;
    while (xrate)
    {
        for (i = 0; i < 1; i++);
        xrate--;
    }
}


void driver_delay_xms(unsigned long xms)
{
    HAL_Delay(xms);
}


void SPI_Write(unsigned char value)
{
    unsigned char i;

    SPI_Delay(1);
    for (i = 0; i < 8; i++)
    {
        EPD_W21_CLK_0;
        SPI_Delay(1);
        if (value & 0x80)
            EPD_W21_MOSI_1;
        else
            EPD_W21_MOSI_0;
        value = (value << 1);
        SPI_Delay(1);
        EPD_W21_CLK_1;
        SPI_Delay(1);
    }
}


const unsigned char LUTDefault_part[31] = {
        0x32,    // command

        0x10, 0x18, 0x18, 0x28, 0x18, 0x18, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x13, 0x11, 0x22, 0x63, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00
};


const unsigned char LUTDefault_full[31] = {
        0x32,    // command
        0x66,   //machine LUT
        0x66,
        0x44,
        0x66,
        0xAA,
        0x11,
        0x80,
        0x08,
        0x11,
        0x18,
        0x81,
        0x18,
        0x11,
        0x88,
        0x11,
        0x88,
        0x11,
        0x88,
        0x00,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0x5F,
        0xAF,
        0xFF,
        0xFF,
        0x2F,
        0x00
};


static unsigned char ReadBusy(void)
{
    unsigned long i = 0;


    for (i = 0; i < 400; i++)
    {
        if (isEPD_W21_BUSY == EPD_W21_BUSY_LEVEL)
        {
            return 1;
        }
        driver_delay_xms(10);
    }
    return 0;
}

static void EPD_W21_WriteCMD(unsigned char command)
{
    EPD_W21_CS_0;
    EPD_W21_DC_0;        // command write
    SPI_Write(command);
    EPD_W21_CS_1;
}

static void EPD_W21_WriteCMD_p1(unsigned char command, unsigned char para)
{
    //while(isEPD_W21_BUSY == 1);	// wait
    ReadBusy();

    EPD_W21_CS_0;
    EPD_W21_DC_0;        // command write
    SPI_Write(command);
    EPD_W21_DC_1;        // command write
    SPI_Write(para);
    EPD_W21_CS_1;
}

static void EPD_W21_Write(unsigned char *value, unsigned char datalen)
{
    unsigned char i = 0;
    unsigned char *ptemp;

    ptemp = value;

    EPD_W21_CS_0;
    EPD_W21_DC_0;        // command write

    SPI_Write(*ptemp);
    ptemp++;

    EPD_W21_DC_1;        // data write

    for (i = 0; i < datalen - 1; i++)    // sub the command
    {
        SPI_Write(*ptemp);
        ptemp++;
    }

    EPD_W21_CS_1;

}

static void EPD_W21_WriteDispRam(unsigned char XSize, unsigned int YSize,
                                 unsigned char *Dispbuff)
{
    int i = 0, j = 0;

    if (XSize % 8 != 0)
    {
        XSize = XSize + (8 - XSize % 8);
    }
    XSize = XSize / 8;

    //while(isEPD_W21_BUSY == 1);	//wait
    ReadBusy();

    EPD_W21_CS_0;
    EPD_W21_DC_0;        //command write
    SPI_Write(0x24);

    EPD_W21_DC_1;        //data write
    for (i = 0; i < YSize; i++)
    {
        for (j = 0; j < XSize; j++)
        {
            SPI_Write(*Dispbuff);
            Dispbuff++;
        }
    }

    EPD_W21_CS_1;
}

static void EPD_W21_WriteDispRamMono(unsigned char XSize, unsigned int YSize,
                                     unsigned char dispdata)
{
    int i = 0, j = 0;

    if (XSize % 8 != 0)
    {
        XSize = XSize + (8 - XSize % 8);
    }
    XSize = XSize / 8;
    //while(isEPD_W21_BUSY == 1);	// wait
    ReadBusy();

    EPD_W21_CS_0;
    EPD_W21_DC_0;        // command write
    SPI_Write(0x24);

    EPD_W21_DC_1;        // data write
    for (i = 0; i < YSize; i++)
    {
        for (j = 0; j < XSize; j++)
        {
            SPI_Write(dispdata);
        }
    }

    EPD_W21_CS_1;
}

static void EPD_W21_POWERON(void)
{
    EPD_W21_WriteCMD_p1(0x22, 0xc0);
    EPD_W21_WriteCMD(0x20);
    //EPD_W21_WriteCMD(0xff);
}

static void EPD_W21_SetRamArea(unsigned char Xstart, unsigned char Xend,
                               unsigned char Ystart, unsigned char Ystart1, unsigned char Yend, unsigned char Yend1)
{
    unsigned char RamAreaX[3];    // X start and end
    unsigned char RamAreaY[5];    // Y start and end
    RamAreaX[0] = 0x44;    // command
    RamAreaX[1] = Xstart;
    RamAreaX[2] = Xend;
    RamAreaY[0] = 0x45;    // command
    RamAreaY[1] = Ystart;
    RamAreaY[2] = Ystart1;
    RamAreaY[3] = Yend;
    RamAreaY[4] = Yend1;
    EPD_W21_Write(RamAreaX, sizeof(RamAreaX));
    EPD_W21_Write(RamAreaY, sizeof(RamAreaY));
}

static void EPD_W21_SetRamPointer(unsigned char addrX, unsigned char addrY, unsigned char addrY1)
{
    unsigned char RamPointerX[2];    // default (0,0)
    unsigned char RamPointerY[3];
    RamPointerX[0] = 0x4e;
    RamPointerX[1] = addrX;
    RamPointerY[0] = 0x4f;
    RamPointerY[1] = addrY;
    RamPointerY[2] = addrY1;

    EPD_W21_Write(RamPointerX, sizeof(RamPointerX));
    EPD_W21_Write(RamPointerY, sizeof(RamPointerY));
}

static void part_display(unsigned char RAM_XST, unsigned char RAM_XEND, unsigned char RAM_YST, unsigned char RAM_YST1,
                         unsigned char RAM_YEND, unsigned char RAM_YEND1)
{
    EPD_W21_SetRamArea(RAM_XST, RAM_XEND, RAM_YST, RAM_YST1, RAM_YEND, RAM_YEND1);    /*set w h*/
    EPD_W21_SetRamPointer(RAM_XST, RAM_YST, RAM_YST1);            /*set orginal*/
}

static void EPD_W21_DispInit(void)
{
    EPD_W21_Write(GDOControl, sizeof(GDOControl));    // Pannel configuration, Gate selection
    EPD_W21_Write(softstart, sizeof(softstart));    // X decrease, Y decrease
    EPD_W21_Write(DummyLine, sizeof(DummyLine));    // dummy line per gate
    EPD_W21_Write(Gatetime, sizeof(Gatetime));        // Gage time setting
    EPD_W21_Write(RamDataEntryMode, sizeof(RamDataEntryMode));    // X increase, Y decrease
    EPD_W21_SetRamArea(0x00, (xDot - 1) / 8, (yDot - 1) % 256, (yDot - 1) / 256, 0x00,
                       0x00);    // X-source area,Y-gage area
    EPD_W21_SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);    // set ram
}

static void EPD_W21_Init(void)
{
    EPD_W21_RST_0;        // Module reset
    driver_delay_xms(100);
    EPD_W21_RST_1;
    driver_delay_xms(100);

    EPD_W21_DispInit();        // pannel configure
}

static void EPD_W21_Update(void)
{
    EPD_W21_WriteCMD_p1(0x22, 0xc7);
    EPD_W21_WriteCMD(0x20);
    EPD_W21_WriteCMD(0xff);
}

static void EPD_W21_Update_Part(void)
{
    EPD_W21_WriteCMD_p1(0x22, 0x04);
    //EPD_W21_WriteCMD_p1(0x22,0x08);
    EPD_W21_WriteCMD(0x20);
    EPD_W21_WriteCMD(0xff);
}

static void EPD_W21_WirteLUT(unsigned char *LUTvalue, unsigned char Size)
{
    EPD_W21_Write(LUTvalue, Size);
}


void EPD_init_Full(void)
{
    EPD_W21_Init();            // display
    EPD_W21_WirteLUT((unsigned char *) LUTDefault_full, sizeof(LUTDefault_full));

    EPD_W21_POWERON();
}

void EPD_Dis_Full(unsigned char *DisBuffer, unsigned char Label)
{
    EPD_W21_SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);    // set ram
    if (Label == 0)
    {
        EPD_W21_WriteDispRamMono(xDot, yDot, 0xff);    // white
    } else
    {
        EPD_W21_WriteDispRam(xDot, yDot, (unsigned char *) DisBuffer);    // white
    }
    EPD_W21_Update();

}
