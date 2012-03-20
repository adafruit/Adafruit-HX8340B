//#include <Wire.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#include "Adafruit_GFX.h"
#include "Adafruit_HX8340B.h"

#include "glcdfont.c"

// a 5x7 font table
extern uint8_t PROGMEM font[];

// the most basic function, set a single pixel
void Adafruit_HX8340B::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
  if ((x >= width()) || (y >= height()))
    return;
	
  setposition(x, y, x+1, y+1);
  writedata16(color);	
}

void Adafruit_HX8340B::begin() {
  constructor(176, 220);

  // set pin directions
  pinMode(sid, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(rst, OUTPUT);
  pinMode(cs, OUTPUT);

  clkport     = portOutputRegister(digitalPinToPort(sclk));
  clkpinmask  = digitalPinToBitMask(sclk);
  mosiport    = portOutputRegister(digitalPinToPort(sid));
  mosipinmask = digitalPinToBitMask(sid);
  csport      = portOutputRegister(digitalPinToPort(cs));
  cspinmask   = digitalPinToBitMask(cs);

  // Reset the LCD
  digitalWrite(rst, HIGH);
  delay(100);
  digitalWrite(rst, LOW);
  delay(50);
  digitalWrite(rst, HIGH);
  delay(50);

  HX8340B_command(HX8340B_N_SETEXTCMD); 
  HX8340B_data(0xFF);
  HX8340B_data(0x83);
  HX8340B_data(0x40); 

  HX8340B_command(HX8340B_N_SPLOUT); 
  delay(100);

  HX8340B_command(0xCA);                  // Undocumented register?
  HX8340B_data(0x70);
  HX8340B_data(0x00);
  HX8340B_data(0xD9); 
  HX8340B_data(0x01);
  HX8340B_data(0x11); 
  
  HX8340B_command(0xC9);                  // Undocumented register?
  HX8340B_data(0x90);
  HX8340B_data(0x49);
  HX8340B_data(0x10); 
  HX8340B_data(0x28);
  HX8340B_data(0x28); 
  HX8340B_data(0x10); 
  HX8340B_data(0x00); 
  HX8340B_data(0x06);
  delay(20);

  HX8340B_command(HX8340B_N_SETGAMMAP);
  HX8340B_data(0x60);
  HX8340B_data(0x71);
  HX8340B_data(0x01); 
  HX8340B_data(0x0E);
  HX8340B_data(0x05); 
  HX8340B_data(0x02); 
  HX8340B_data(0x09); 
  HX8340B_data(0x31);
  HX8340B_data(0x0A);
  
  HX8340B_command(HX8340B_N_SETGAMMAN); 
  HX8340B_data(0x67);
  HX8340B_data(0x30);
  HX8340B_data(0x61); 
  HX8340B_data(0x17);
  HX8340B_data(0x48); 
  HX8340B_data(0x07); 
  HX8340B_data(0x05); 
  HX8340B_data(0x33); 
  delay(10);

  HX8340B_command(HX8340B_N_SETPWCTR5); 
  HX8340B_data(0x35);
  HX8340B_data(0x20);
  HX8340B_data(0x45); 
  
  HX8340B_command(HX8340B_N_SETPWCTR4); 
  HX8340B_data(0x33);
  HX8340B_data(0x25);
  HX8340B_data(0x4c); 
  delay(10);

  HX8340B_command(HX8340B_N_COLMOD);  // Color Mode
  HX8340B_data(0x05);                 // 0x05 = 16bpp, 0x06 = 18bpp

  HX8340B_command(HX8340B_N_DISPON); 
  delay(10);

  HX8340B_command(HX8340B_N_CASET); 
  HX8340B_data(0x00);
  HX8340B_data(0x00);
  HX8340B_data(0x00); 
  HX8340B_data(0xaf);                 // 175

  HX8340B_command(HX8340B_N_PASET); 
  HX8340B_data(0x00);
  HX8340B_data(0x00);
  HX8340B_data(0x00); 
  HX8340B_data(0xdb);                // 219

  HX8340B_command(HX8340B_N_RAMWR);   
}

// clear everything
void Adafruit_HX8340B::clearDisplay(void) {
  uint8_t i,j;
  for (i=0;i<220;i++)
  {
    for (j=0;j<176;j++)
    {
      writedata16(0x0000);
    }
  }
}

void Adafruit_HX8340B::invertDisplay(uint8_t i) {
}

void Adafruit_HX8340B::HX8340B_command(uint8_t c) { 
  digitalWrite(cs, LOW);

  digitalWrite(sid, LOW);
  digitalWrite(sclk, LOW);
  digitalWrite(sclk, HIGH);

  uint8_t i = 0;
  for (i=0; i<8; i++) 
  { 
    if (c & 0x80) 
    { 
      digitalWrite(sid, HIGH);
    } 
    else 
    { 
      digitalWrite(sid, LOW);
    } 
    digitalWrite(sclk, LOW);
    c <<= 1; 
	digitalWrite(sclk, HIGH);
  } 
  digitalWrite(cs, HIGH);
}

void Adafruit_HX8340B::HX8340B_data(uint8_t c) {
  digitalWrite(cs, LOW);

  digitalWrite(sid, HIGH);
  digitalWrite(sclk, LOW);
  digitalWrite(sclk, HIGH);

  uint8_t i = 0;
  for (i=0; i<8; i++) 
  { 
    if (c & 0x80) 
    { 
      digitalWrite(sid, HIGH);
    } 
    else 
    { 
      digitalWrite(sid, LOW);
    } 
    digitalWrite(sclk, LOW);
    c <<= 1; 
	digitalWrite(sclk, HIGH);
  } 
  digitalWrite(cs, HIGH);
}

void Adafruit_HX8340B::writedata16(uint16_t data) { 
  HX8340B_data((data>>8) & 0xFF);
  HX8340B_data(data & 0xFF);
}

void Adafruit_HX8340B::writereg(uint8_t reg, uint8_t value) { 
  HX8340B_command(reg);
  HX8340B_command(value);
}

void Adafruit_HX8340B::setposition(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
{
  HX8340B_command(HX8340B_N_CASET);
  HX8340B_data(x0>>8);
  HX8340B_data(x0);
  HX8340B_data(x1>>8);
  HX8340B_data(x1);
  
  HX8340B_command(HX8340B_N_PASET);
  HX8340B_data(y0>>8);
  HX8340B_data(y0);
  HX8340B_data(y1>>8);
  HX8340B_data(y1);

  HX8340B_command(HX8340B_N_RAMWR);
}
