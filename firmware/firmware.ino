#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "num2str.h"
#include "num2str.c"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "Bold.h"
using namespace Bold;

// try 8Mhz

#define LEDPIN   1
#define BUTTON1  3
#define BUTTON2  4

#define OFFSEC     4
#define DELAY_TENTH 100 // should be 100ms

int hours   = 0;
int minutes = 0;
int seconds = 0;

int onsec    = 0;
byte tick    = 0;
byte ledon   = 0;

byte menu = 0;

int vcc = 3730;

void myFont(byte x, byte b) {
  int y=0;
  if (b == 0) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num0);
  } else if (b == 1) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num1);
  } else if (b == 2) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num2);
  } else if (b == 3) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num3);
  } else if (b == 4) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num4);
  } else if (b == 5) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num5);
  } else if (b == 6) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num6);
  } else if (b == 7) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num7);
  } else if (b == 8) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num8);
  } else if (b == 9) {
    ssd1306_draw_bmp(x, 0, x+16, 8, num9);
  }
}

inline void readVcc() {
  // read vcc
  power_adc_enable();
  ADMUX = (0<<REFS0) | (12<<MUX0);
  ssd1306_fill(0); // is a delay
  ADCSRA |= (1<<ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  vcc = ADCW;
  vcc = 1125300L / vcc;
  power_adc_disable();

  ssd1306_setpos(0,0);  
  ssd1306_numdec(vcc); 
}

inline void bigDigital() {
  int t = hours/10;
  myFont(0, t);
  t = hours - t*10;
  myFont(16, t);
  
  t = minutes/10;
  myFont(36, t);
  t = minutes - t*10;
  myFont(52, t);

  t = seconds/10;
  myFont(77, t);
  t = seconds - t*10;
  myFont(93, t);
  
  myFont(111, tick);
}

inline void ticking() {
  tick++;
  if (tick > 9) {
    seconds += tick/10;
    if (onsec>=0 && onsec <= OFFSEC) onsec++;
  }
  
  if (tick > 9) {
    tick = tick % 10;
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
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(LEDPIN,  OUTPUT);
  wdt_disable();
  sleep_bod_disable();
  power_adc_disable();
  power_timer1_disable();
  
  ssd1306_init();
  ssd1306_fill(0);
  delay(500);
}


void loop() {
  delay(DELAY_TENTH);
  ticking();

  // --------------------------------------------------
  if (menu==0) {
    
    if (onsec > OFFSEC) {
      digitalWrite(LEDPIN, LOW);
      ssd1306_off();
      onsec = -1;
    } else {
      if (onsec >= 0) {
        digitalWrite(LEDPIN, LOW);
        bigDigital();
      }
    }
    
    if (digitalRead(BUTTON1) == LOW) {
      onsec = 0;
      ssd1306_on();
      bigDigital();
    } 

  // --------------------------------------------------
  } else if (menu==1) {
    
    digitalWrite(LEDPIN, HIGH);
    
  // --------------------------------------------------
  } else if (menu==2) {
    ssd1306_on();
    readVcc();
    ssd1306_string_font6x8(" mV");
    ssd1306_setpos(0,2);
    ssd1306_string_font6x8("set hour:  ");
    digitalWrite(LEDPIN, LOW);
    if (digitalRead(BUTTON1) == LOW) {
      hours = (hours+1)%24;
      tick=0;
      seconds=0;
    }
    //ssd1306_setpos(0,2);
    ssd1306_numdec(hours);
    
  // --------------------------------------------------
  } else if (menu==3) {
    ssd1306_on();
    readVcc();
    ssd1306_string_font6x8(" mV");
    ssd1306_setpos(0,2);
    ssd1306_string_font6x8("set minute:  ");
    digitalWrite(LEDPIN, LOW);
    if (digitalRead(BUTTON1) == LOW) {
      minutes = (minutes+1)%60;
      tick=0;
      seconds=0;
    }
    //ssd1306_setpos(0,3);
    ssd1306_numdec(minutes);
  }


  if (digitalRead(BUTTON2) == LOW) {
    ssd1306_fill(0);
    menu = (menu+1)%4;
  }
}
