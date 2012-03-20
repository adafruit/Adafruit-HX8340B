#define OLED_CS 11
#define OLED_CLK 10
#define OLED_MOSI 9
#define OLED_RESET 12

#include <Adafruit_GFX.h>
#include <Adafruit_HX8340B.h>

Adafruit_HX8340B display(OLED_MOSI, OLED_CLK, OLED_RESET, OLED_CS);

void setup()   {                
  Serial.begin(9600);
  
  display.begin();
  // init done
  
  display.clearDisplay();   // clears the screen and buffer

  // draw a single pixel
  display.drawPixel(10, 10, WHITE);

  // draw mulitple circles
  testdrawcircle();
  delay(2000);
  display.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  delay(2000);
  display.clearDisplay();

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  delay(2000);
}


void loop() {
  
}

void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
}

void testdrawcircle(void) {
  for (uint8_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
  }
}
