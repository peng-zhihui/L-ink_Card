#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

extern void driver_delay_xms(unsigned long xms);

extern void EPD_Dis_Part(unsigned char xStart, unsigned char xEnd, unsigned long yStart, unsigned long yEnd,
                         unsigned char *DisBuffer, unsigned char Label);

extern void EPD_Dis_Full(unsigned char *DisBuffer, unsigned char Label);

extern void EPD_init_Full(void);

extern void EPD_init_Part(void);

extern void SPI_Write(unsigned char value);

extern void driver_delay_xms(unsigned long xms);

#define EPD_W21_MOSI_0    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET)
#define EPD_W21_MOSI_1    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET)

#define EPD_W21_CLK_0    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET)
#define EPD_W21_CLK_1    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET)

#define EPD_W21_CS_0    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET)
#define EPD_W21_CS_1    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET)

#define EPD_W21_DC_0    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET)
#define EPD_W21_DC_1    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET)

#define EPD_W21_RST_0    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET)
#define EPD_W21_RST_1    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET)


#define EPD_W21_BUSY_LEVEL 0
#define isEPD_W21_BUSY HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)

#endif
/***********************************************************
						end file
***********************************************************/


