#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "num2str.h"
#include "num2str.c"

byte hours   = 0;
byte minutes = 0;
byte seconds = 0;
byte tick    = 0;

void ticking() {
  tick++;
  if (tick > 3) {
    seconds += tick/4;
  }
  
  if (tick > 3) {
    tick = tick % 4;
    if (seconds > 59) {
      minutes += seconds / 60;
      seconds  = seconds % 60;
    }
    if (minutes > 59) {
      hours  += minutes / 60;
      minutes = minutes % 60;
    }
    if (hours > 23) {
      hours = hours % 24;
    }
  }
}

void setup() {
  ssd1306_init();
  ssd1306_fill(0);
  delay(500);
  ssd1306_setpos(0,0);
}

void loop() {
  ticking();
  delay(250);
  ssd1306_string("Test");
  if (hours < 10) ssd1306_char('0');
  ssd1306_numdec(hours);
  ssd1306_char(':');
  if (minutes < 10) ssd1306_char('0');
  ssd1306_numdec(minutes);
  ssd1306_char(':');
  if (seconds < 10) ssd1306_char('0');
  ssd1306_numdec(seconds);
  ssd1306_setpos(0,0);
}
