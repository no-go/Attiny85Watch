#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "ssd1306xled8x16.h"
#include "ssd1306xled8x16.c"
#include "num2str.h"
#include "num2str.c"

// tested: 14h with 1MHz 65mAh Lipo

#define LEDPIN   1
#define BUTTON1  4
#define BUTTON2  3

#define OFFSEC   6

#define POWERMAX 3900
#define POWERMIN 3180

int hours   = 0;
int minutes = 0;
int seconds = 2;

int onsec    = 0;
byte tick    = 0;
bool ledon = false;

int vcc = 3700;

const byte smallBitmap[] PROGMEM = {
  0b00010000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00010000,
  0b00011100,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00001000,
  0b00010000
};

void setup() {
  ssd1306_init();
  ssd1306_fill(0);
  delay(500);
}

void loop() {
  ssd1306_draw_bmp(0,  0, 16, 8,  smallBitmap);
  ssd1306_draw_bmp(44, 0, 76, 2,  smallBitmap);
}
