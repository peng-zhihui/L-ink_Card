#ifndef _DISPLAY_EPD_W21_CONFIG_H_
#define _DISPLAY_EPD_W21_CONFIG_H_

#include "Display_EPD_W21.h"

#define xDot 200
#define yDot 200

unsigned char GDOControl[] = {0x01, (yDot - 1) % 256, (yDot - 1) / 256, 0x00}; //for 1.54inch
unsigned char softstart[] = {0x0c, 0xd7, 0xd6, 0x9d};
unsigned char Rambypass[] = {0x21, 0x8f};        // Display update
unsigned char MAsequency[] = {0x22, 0xf0};        // clock
unsigned char GDVol[] = {0x03, 0x66};    // Gate voltage +18V/-18V
unsigned char SDVol[] = {0x04, 0x04};    // Source voltage +12V/-12V
unsigned char VCOMVol[] = {0x2c, 0x7f};    // VCOM 7c
unsigned char BOOSTERFB[] = {0xf0, 0x1f};    // Source voltage +15V/-15V
unsigned char DummyLine[] = {0x3a, 0x1a};    // 4 dummy line per gate
unsigned char Gatetime[] = {0x3b, 0x08};    // 2us per line
unsigned char BorderWavefrom[] = {0x3c, 0x33};    // Border
unsigned char RamDataEntryMode[] = {0x11, 0x01};    // Ram data entry mode


#endif

