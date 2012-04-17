#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include <SPI.h>

#include "Adafruit_GFX.h"
#include "Adafruit_HX8340B.h"

// use bitbang SPI (not suggested)
Adafruit_HX8340B::Adafruit_HX8340B(int8_t SID, int8_t SCLK, int8_t RST, int8_t CS) {
  sid = SID;
  sclk = SCLK;
  rst = RST;
  cs = CS;
  hwSPI = false;
}

// use hardware SPI
Adafruit_HX8340B::Adafruit_HX8340B(int8_t RST, int8_t CS) {
  sid = -1;
  sclk = -1;
  rst = RST;
  cs = CS;
  hwSPI = true;
}


void Adafruit_HX8340B::begin() {
  // Constructor for underlying GFX library
  constructor(HX8340B_LCDWIDTH, HX8340B_LCDHEIGHT);

  // set pin directions
  if (! hwSPI) {
    pinMode(sid, OUTPUT);
    pinMode(sclk, OUTPUT);
  }
  pinMode(rst, OUTPUT);
  pinMode(cs, OUTPUT);

  // Set pins low by default (except reset)
  if (! hwSPI) {
    digitalWrite(sid, LOW);
    digitalWrite(sclk, LOW);
  }
  digitalWrite(cs, LOW);
  digitalWrite(rst, HIGH);

  // Reset the LCD
  digitalWrite(rst, HIGH);
  delay(100);
  digitalWrite(rst, LOW);
  delay(50);
  digitalWrite(rst, HIGH);
  delay(50);


  csport    = portOutputRegister(digitalPinToPort(cs));
  cspinmask = digitalPinToBitMask(cs);

  if (! hwSPI) {
    clkport    = portOutputRegister(digitalPinToPort(sclk));
    clkpinmask = digitalPinToBitMask(sclk);
    dataport    = portOutputRegister(digitalPinToPort(sid));
    datapinmask = digitalPinToBitMask(sid);
  } else {
    clkport    = portOutputRegister(digitalPinToPort(13));
    clkpinmask = digitalPinToBitMask(13);
    dataport    = portOutputRegister(digitalPinToPort(11));
    datapinmask = digitalPinToBitMask(11);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV8); // 4 MHz (half speed)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    pinMode(13, OUTPUT);
    pinMode(11, OUTPUT);
  }

  *csport &= ~cspinmask;
  HX8340B_command(HX8340B_N_SETEXTCMD); 
  writeData(0xFF);
  writeData(0x83);
  writeData(0x40); 

  HX8340B_command(HX8340B_N_SPLOUT); 
  delay(100);

  HX8340B_command(0xCA);                  // Undocumented register?
  writeData(0x70);
  writeData(0x00);
  writeData(0xD9); 
  writeData(0x01);
  writeData(0x11); 
  
  HX8340B_command(0xC9);                  // Undocumented register?
  writeData(0x90);
  writeData(0x49);
  writeData(0x10); 
  writeData(0x28);
  writeData(0x28); 
  writeData(0x10); 
  writeData(0x00); 
  writeData(0x06);
  delay(20);

  HX8340B_command(HX8340B_N_SETGAMMAP);
  writeData(0x60);
  writeData(0x71);
  writeData(0x01); 
  writeData(0x0E);
  writeData(0x05); 
  writeData(0x02); 
  writeData(0x09); 
  writeData(0x31);
  writeData(0x0A);
  
  HX8340B_command(HX8340B_N_SETGAMMAN); 
  writeData(0x67);
  writeData(0x30);
  writeData(0x61); 
  writeData(0x17);
  writeData(0x48); 
  writeData(0x07); 
  writeData(0x05); 
  writeData(0x33); 
  delay(10);

  HX8340B_command(HX8340B_N_SETPWCTR5); 
  writeData(0x35);
  writeData(0x20);
  writeData(0x45); 
  
  HX8340B_command(HX8340B_N_SETPWCTR4); 
  writeData(0x33);
  writeData(0x25);
  writeData(0x4c); 
  delay(10);

  HX8340B_command(HX8340B_N_COLMOD);  // Color Mode
  writeData(0x05);                 // 0x05 = 16bpp, 0x06 = 18bpp

  HX8340B_command(HX8340B_N_DISPON); 
  delay(10);

  HX8340B_command(HX8340B_N_CASET); 
  writeData(0x00);
  writeData(0x00);
  writeData(0x00); 
  writeData(0xaf);                 // 175

  HX8340B_command(HX8340B_N_PASET); 
  writeData(0x00);
  writeData(0x00);
  writeData(0x00); 
  writeData(0xdb);                 // 219

  HX8340B_command(HX8340B_N_RAMWR);
  
  *csport |=  cspinmask;
  //clearDisplay();
}

// clear everything
void Adafruit_HX8340B::fillDisplay(uint16_t c) {
  fillRect(0, 0, HX8340B_LCDWIDTH, HX8340B_LCDHEIGHT, c);
}

void Adafruit_HX8340B::invertDisplay(uint8_t i) {
}

void Adafruit_HX8340B::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  setWindow(x, y, x+w-1, y+h-1);

  *csport &= ~cspinmask;

  uint32_t i = w;
  i *= h;

  while (i--)
  {
    writeData((color>>8) & 0xFF);
    writeData(color & 0xFF);
  }

  *csport |=  cspinmask;
}

void Adafruit_HX8340B::pushColor(uint16_t color) {
  *csport &= ~cspinmask;
  writeData((color>>8) & 0xFF);
  writeData(color & 0xFF);
  *csport |=  cspinmask;
}

// the most basic function, set a single pixel
void Adafruit_HX8340B::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;
	
  setWindow(x, y, x+1, y+1);
  *csport &= ~cspinmask;
  writeData((color>>8) & 0xFF);
  writeData(color & 0xFF);
  *csport |=  cspinmask;
}


void Adafruit_HX8340B::HX8340B_command(uint8_t c) { 
  // Prepend leading bit instead of D/C pin

  if (hwSPI) {
    uint8_t saved_spimode = SPCR;
    SPCR = 0;

#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined(__AVR_ATmega8__)
    PORTB &= ~_BV(3);  // PB3 = MOSI
    PORTB |= _BV(5);  // PB5 = SCLK
    PORTB &= ~_BV(5);
    // also do mega next eh?
#else
    *dataport &=  ~datapinmask;
    *clkport |=  clkpinmask;
    *clkport &= ~clkpinmask;
#endif

    SPCR = saved_spimode;

    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
  } else {
    *dataport &=  ~datapinmask;
    *clkport |=  clkpinmask;
    *clkport &= ~clkpinmask;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) *dataport |=  datapinmask;
      else        *dataport &= ~datapinmask;
      *clkport |=  clkpinmask;
      *clkport &= ~clkpinmask;
    }
  }
}


void Adafruit_HX8340B::writeData(uint8_t c) {
  // Prepend leading bit instead of D/C pin
  if (hwSPI) {
    uint8_t saved_spimode = SPCR;
    SPCR = 0;
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined(__AVR_ATmega8__)
    PORTB |= _BV(3);  // PB3 = MOSI
    PORTB |= _BV(5);  // PB5 = SCLK
    PORTB &= ~_BV(5);
    // also do mega next eh?
#else
    *dataport |=  datapinmask;
    *clkport |=  clkpinmask;
    *clkport &= ~clkpinmask;
#endif
    SPCR = saved_spimode;
    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
  } else {
    *dataport |=  datapinmask;
    *clkport |=  clkpinmask;
    *clkport &= ~clkpinmask;
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) *dataport |=  datapinmask;
      else        *dataport &= ~datapinmask;
      *clkport |=  clkpinmask;
      *clkport &= ~clkpinmask;
    }
  }
}

void Adafruit_HX8340B::writereg(uint8_t reg, uint8_t value) { 
  *csport &= ~cspinmask;
  HX8340B_command(reg);
  HX8340B_command(value);
  *csport |=  cspinmask;
}

void Adafruit_HX8340B::setWindow(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1)
{
  *csport &= ~cspinmask;
  HX8340B_command(HX8340B_N_CASET);
  writeData(x0>>8);
  writeData(x0);
  writeData(x1>>8);
  writeData(x1);
  
  HX8340B_command(HX8340B_N_PASET);
  writeData(y0>>8);
  writeData(y0);
  writeData(y1>>8);
  writeData(y1);

  HX8340B_command(HX8340B_N_RAMWR);
  *csport |=  cspinmask;
}

uint16_t Adafruit_HX8340B::Color565(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;
  c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;

  return c;
}
