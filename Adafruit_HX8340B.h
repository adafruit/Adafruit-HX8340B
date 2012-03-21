
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_GFX.h>

/**************************************************************************
    BTL221722-276L CONNECTOR (HX8340B_ICVERSION_N)
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      1   VSS
      2   NC
      3   LED A/+         Vf 3.3V
      4   VDD             2.8-3.3V
      5   CS
      6   SDI             Serial Data
      7   SCL             Serial Clock
      8   RS              Not used
      9   RESET
     10   VDD             2.8-3.3V
     11   LED K1
     12   LED K2
     13   LED K3
     14   VSS

 **************************************************************************/

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

#define BLACK 0
#define WHITE 1

#define HX8340B_LCDWIDTH                  128
#define HX8340B_LCDHEIGHT                 64

// HX8340-B(N) Commands (used by BTL221722-276L)
#define HX8340B_N_NOP                     (0x00)
#define HX8340B_N_SWRESET                 (0x01)
#define HX8340B_N_RDDIDIF                 (0x04)
#define HX8340B_N_RDDST                   (0x09)
#define HX8340B_N_RDDPM                   (0x0A)
#define HX8340B_N_RDDMADCTL               (0x0B)
#define HX8340B_N_RDDCOLMOD               (0x0C)
#define HX8340B_N_RDDIM                   (0x0D)
#define HX8340B_N_RDDSM                   (0x0E)
#define HX8340B_N_RDDSDR                  (0x0F)
#define HX8340B_N_SLPIN                   (0x10)
#define HX8340B_N_SPLOUT                  (0x11)
#define HX8340B_N_PTLON                   (0x12)
#define HX8340B_N_NORON                   (0x13)
#define HX8340B_N_INVOFF                  (0x20)
#define HX8340B_N_INVON                   (0x21)
#define HX8340B_N_GAMSET                  (0x26)
#define HX8340B_N_DISPOFF                 (0x28)
#define HX8340B_N_DISPON                  (0x29)
#define HX8340B_N_CASET                   (0x2A)
#define HX8340B_N_PASET                   (0x2B)
#define HX8340B_N_RAMWR                   (0x2C)
#define HX8340B_N_RAMRD                   (0x2E)
#define HX8340B_N_RGBSET                  (0x2D)
#define HX8340B_N_PLTAR                   (0x30)
#define HX8340B_N_VSCRDEF                 (0x33)
#define HX8340B_N_TEOFF                   (0x34)
#define HX8340B_N_TEON                    (0x35)
#define HX8340B_N_MADCTL                  (0x36)
#define HX8340B_N_VSCRSADD                (0x37)
#define HX8340B_N_IDMOFF                  (0x38)
#define HX8340B_N_IDMON                   (0x39)
#define HX8340B_N_COLMOD                  (0x3A)
#define HX8340B_N_RDID1                   (0xDA)
#define HX8340B_N_RDID2                   (0xDB)
#define HX8340B_N_RDID3                   (0xDC)
#define HX8340B_N_SETOSC                  (0xB0)
#define HX8340B_N_SETPWCTR1               (0xB1)
#define HX8340B_N_SETPWCTR2               (0xB2)
#define HX8340B_N_SETPWCTR3               (0xB3)
#define HX8340B_N_SETPWCTR4               (0xB4)
#define HX8340B_N_SETPWCTR5               (0xB5)
#define HX8340B_N_SETDISCTRL              (0xB6)
#define HX8340B_N_SETFRMCTRL              (0xB7)
#define HX8340B_N_SETDISCYCCTRL           (0xB8)
#define HX8340B_N_SETINVCTRL              (0xB9)
#define HX8340B_N_RGBBPCTR                (0xBA)
#define HX8340B_N_SETRGBIF                (0xBB)
#define HX8340B_N_SETDODC                 (0xBC)
#define HX8340B_N_SETINTMODE              (0xBD)
#define HX8340B_N_SETPANEL                (0xBE)
#define HX8340B_N_SETOTP                  (0xC7)
#define HX8340B_N_SETONOFF                (0xC0)
#define HX8340B_N_SETEXTCMD               (0xC1)
#define HX8340B_N_SETGAMMAP               (0xC2)
#define HX8340B_N_SETGAMMAN               (0xC3)

class Adafruit_HX8340B : public Adafruit_GFX {
 public:
  Adafruit_HX8340B(int8_t SID, int8_t SCLK, int8_t RST, int8_t CS) : sid(SID), sclk(SCLK), rst(RST), cs(CS) {}

  void begin();
  void HX8340B_command(uint8_t c);
  void HX8340B_data(uint8_t c);

  void clearDisplay(void);
  void invertDisplay(uint8_t i);
  void display();

  void drawPixel(uint16_t x, uint16_t y, uint16_t color);

 private:
  int8_t sid, sclk, rst, cs;
  void writereg(uint8_t reg, uint8_t value);
  void setposition(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
};
