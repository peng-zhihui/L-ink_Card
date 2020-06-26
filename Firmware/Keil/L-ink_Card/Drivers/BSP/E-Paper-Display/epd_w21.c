#include "epd_w21_config.h"
#include "stm32l0xx_hal.h"

void SpiDelay(unsigned char xrate)
{
    unsigned char i;
    while (xrate)
    {
        for (i = 0; i < 1; i++);
        xrate--;
    }
}


void DriverDelay(unsigned long xms)
{
    HAL_Delay(xms);
}


void SpiWrite(unsigned char value)
{
    unsigned char i;

    SpiDelay(1);
    for (i = 0; i < 8; i++)
    {
        EPD_W21_CLK_0;
        SpiDelay(1);
        if (value & 0x80)
            EPD_W21_MOSI_1;
        else
            EPD_W21_MOSI_0;
        value = (value << 1);
        SpiDelay(1);
        EPD_W21_CLK_1;
        SpiDelay(1);
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
        DriverDelay(10);
    }
    return 0;
}

static void EpdW21WriteCMD(unsigned char command)
{
    EPD_W21_CS_0;
    EPD_W21_DC_0;        // command write
    SpiWrite(command);
    EPD_W21_CS_1;
}

static void EpdW21WriteCMD_p1(unsigned char command, unsigned char para)
{
    //while(isEPD_W21_BUSY == 1);	// wait
    ReadBusy();

    EPD_W21_CS_0;
    EPD_W21_DC_0;        // command write
    SpiWrite(command);
    EPD_W21_DC_1;        // command write
    SpiWrite(para);
    EPD_W21_CS_1;
}

static void EpdW21Write(unsigned char *value, unsigned char datalen)
{
    unsigned char i = 0;
    unsigned char *ptemp;

    ptemp = value;

    EPD_W21_CS_0;
    EPD_W21_DC_0;        // command write

    SpiWrite(*ptemp);
    ptemp++;

    EPD_W21_DC_1;        // data write

    for (i = 0; i < datalen - 1; i++)    // sub the command
    {
        SpiWrite(*ptemp);
        ptemp++;
    }

    EPD_W21_CS_1;

}

static void EpdW21WriteDispRam(unsigned char XSize, unsigned int YSize,
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
    SpiWrite(0x24);

    EPD_W21_DC_1;        //data write
    for (i = 0; i < YSize; i++)
    {
        for (j = 0; j < XSize; j++)
        {
            SpiWrite(*Dispbuff);
            Dispbuff++;
        }
    }

    EPD_W21_CS_1;
}

static void EpdW21WriteDispRamMono(unsigned char XSize, unsigned int YSize,
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
    SpiWrite(0x24);

    EPD_W21_DC_1;        // data write
    for (i = 0; i < YSize; i++)
    {
        for (j = 0; j < XSize; j++)
        {
            SpiWrite(dispdata);
        }
    }

    EPD_W21_CS_1;
}

static void EpdW21PowerOn(void)
{
    EpdW21WriteCMD_p1(0x22, 0xc0);
    EpdW21WriteCMD(0x20);
    //EpdW21WriteCMD(0xff);
}

static void EpdW21SetRamArea(unsigned char Xstart, unsigned char Xend,
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
    EpdW21Write(RamAreaX, sizeof(RamAreaX));
    EpdW21Write(RamAreaY, sizeof(RamAreaY));
}

static void EpdW21SetRamPointer(unsigned char addrX, unsigned char addrY, unsigned char addrY1)
{
    unsigned char RamPointerX[2];    // default (0,0)
    unsigned char RamPointerY[3];
    RamPointerX[0] = 0x4e;
    RamPointerX[1] = addrX;
    RamPointerY[0] = 0x4f;
    RamPointerY[1] = addrY;
    RamPointerY[2] = addrY1;

    EpdW21Write(RamPointerX, sizeof(RamPointerX));
    EpdW21Write(RamPointerY, sizeof(RamPointerY));
}

static void PartDisplay(unsigned char RAM_XST, unsigned char RAM_XEND, unsigned char RAM_YST, unsigned char RAM_YST1,
                        unsigned char RAM_YEND, unsigned char RAM_YEND1)
{
    EpdW21SetRamArea(RAM_XST, RAM_XEND, RAM_YST, RAM_YST1, RAM_YEND, RAM_YEND1);    /*set w h*/
    EpdW21SetRamPointer(RAM_XST, RAM_YST, RAM_YST1);            /*set orginal*/
}

static void EpdW21DispInit(void)
{
    EpdW21Write(GDOControl, sizeof(GDOControl));    // Pannel configuration, Gate selection
    EpdW21Write(softStart, sizeof(softStart));    // X decrease, Y decrease
    EpdW21Write(dummyLine, sizeof(dummyLine));    // dummy line per gate
    EpdW21Write(gateTime, sizeof(gateTime));        // Gage time setting
    EpdW21Write(ramDataEntryMode, sizeof(ramDataEntryMode));    // X increase, Y decrease
    EpdW21SetRamArea(0x00, (xDot - 1) / 8, (yDot - 1) % 256, (yDot - 1) / 256, 0x00,
                     0x00);    // X-source area,Y-gage area
    EpdW21SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);    // set ram
}

static void EpdW21Init(void)
{
    EPD_W21_RST_0;        // Module reset
    DriverDelay(100);
    EPD_W21_RST_1;
    DriverDelay(100);

    EpdW21DispInit();        // pannel configure
}

static void EpdW21Update(void)
{
    EpdW21WriteCMD_p1(0x22, 0xc7);
    EpdW21WriteCMD(0x20);
    EpdW21WriteCMD(0xff);
}

static void EpdW21UpdatePart(void)
{
    EpdW21WriteCMD_p1(0x22, 0x04);
    //EpdW21WriteCMD_p1(0x22,0x08);
    EpdW21WriteCMD(0x20);
    EpdW21WriteCMD(0xff);
}

static void EpdW21WirteLUT(unsigned char *LUTvalue, unsigned char Size)
{
    EpdW21Write(LUTvalue, Size);
}


void EpdInitFull(void)
{
    EpdW21Init();            // display
    EpdW21WirteLUT((unsigned char *) LUTDefault_full, sizeof(LUTDefault_full));

    EpdW21PowerOn();
}

void EpdDisFull(unsigned char *DisBuffer, unsigned char Label)
{
    EpdW21SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);    // set ram
    if (Label == 0)
    {
        EpdW21WriteDispRamMono(xDot, yDot, 0xff);    // white
    } else
    {
        EpdW21WriteDispRam(xDot, yDot, (unsigned char *) DisBuffer);    // white
    }
    EpdW21Update();

}
