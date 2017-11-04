#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "num2str.h"
#include "num2str.c"

#define LEDPIN   1
#define HOURPIN  4
#define MINUPIN  3

#define OFFSEC   5

int hours   = 0;
int minutes = 0;
int seconds = 0;
int onsec   = 0;
byte tick    = 0;
bool ledon = false;

inline void ticking() {
  tick++;
  if (tick > 3) {
    seconds += tick/4;
    if (onsec>=0 && onsec <= OFFSEC) onsec++;
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
  pinMode(HOURPIN, INPUT_PULLUP);
  pinMode(MINUPIN, INPUT_PULLUP);
  pinMode(LEDPIN,  OUTPUT);
  
  ssd1306_init();
  ssd1306_fill(0);
  delay(500);
  ssd1306_setpos(0,0);
}

void loop() {
  ticking();
  delay(250);
  ssd1306_setpos(20,1);
  if (hours < 10) ssd1306_char('0');
  ssd1306_numdec(hours);
  ssd1306_char(':');
  if (minutes < 10) ssd1306_char('0');
  ssd1306_numdec(minutes);
  ssd1306_char(':');
  if (seconds < 10) ssd1306_char('0');
  ssd1306_numdec(seconds);
  
  if (digitalRead(HOURPIN) == LOW) {
    ledon = !ledon;
    onsec = 0;
    ssd1306_on();
    if (ledon == true) {
      digitalWrite(LEDPIN, HIGH);
    } else {
      digitalWrite(LEDPIN, LOW);      
    }
    
    delay(1000);
    tick+=4;
    if (digitalRead(HOURPIN) == LOW) {
      hours = (hours+1)%24;
      seconds = 0;
    }
  }
  
  if (digitalRead(MINUPIN) == LOW) {
    onsec = 0;
    ssd1306_on();
    delay(250);
    tick++;
    if (digitalRead(MINUPIN) == LOW) {
      minutes = (minutes+1)%60;
      seconds = 0;
    }
  }

  if (onsec > OFFSEC) {
    ssd1306_off();
    onsec = -1;
  }
}
