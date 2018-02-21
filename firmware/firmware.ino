#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "num2str.h"
#include "num2str.c"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//#include "Bold.h"
#include "Normal.h"
#include "Hglas.h"
using namespace Normal;

// with 8Mhz -> 4h  :-(  4.1 V till 3.5  V
// with 1Mhz -> 15h      4.1 V till 3.58 V

#define LEDPIN   1
#define BUTTON1  3
#define BUTTON2  4

#define         OFFSEC      5
#define         DELAY_TENTH 100 // should be 100ms
int tickDelay = DELAY_TENTH;

int hours   = 0;
int minutes = 0;
int seconds = 0;
int alarms =  0;

int onsec    = 0;
byte tick    = 0;
byte ledon   = 0;

// old to detect refresh
int ohours   = 23;
int ominutes = 58;
int oseconds = 50;
byte otick   = 3;

byte menu = 0;

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

void myFont2(byte x, byte b) {
  int y=0;
  if (b == 0) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num0);
  } else if (b == 1) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num1);
  } else if (b == 2) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num2);
  } else if (b == 3) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num3);
  } else if (b == 4) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num4);
  } else if (b == 5) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num5);
  } else if (b == 6) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num6);
  } else if (b == 7) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num7);
  } else if (b == 8) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num8);
  } else if (b == 9) {
    ssd1306_draw_bmp(x, 0, x+16, 8, Hglas::num9);
  }
}

inline void readVcc() {
  // read vcc
  power_adc_enable();
  ADMUX = (0<<REFS0) | (12<<MUX0);
  ssd1306_fill(0); // is a delay
  ADCSRA |= (1<<ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  int vcc = ADCW;
  vcc = 1125300L / vcc;
  power_adc_disable();

  ssd1306_setpos(0,0);  
  ssd1306_numdec(vcc); 
}

inline void bigDigital() {
  int t;
  if (hours != ohours) {
    ohours=hours;
    t = hours/10;
    myFont(0, t);
    t = hours - t*10;
    myFont(16, t);
  }
    
  if (minutes != ominutes) {
    ominutes=minutes;
    t = minutes/10;
    myFont(36, t);
    t = minutes - t*10;
    myFont(52, t);
  }
  
  if (seconds != oseconds) {
    oseconds=seconds;
    t = seconds/10;
    myFont(77, t);
    t = seconds - t*10;
    myFont(93, t);
  }
    
  if (tick != otick) {
    otick=tick;
    myFont2(111, tick);
  }
}

inline void ticking() {
  tick++;
  if (tick > 9) {
    seconds += tick/10;
    if (onsec>=0 && onsec <= OFFSEC) onsec++;
  }
  
  
  if (tick > 9) {
    if (alarms > 1) alarms--;
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
  delay(tickDelay);
  ticking();

  if (alarms==1) {
    if(tick==3) {
      digitalWrite(LEDPIN, HIGH);
    } else {
      digitalWrite(LEDPIN, LOW);
    }
  }
  
  // --------------------------------------------------
  if (menu==0) {
    
    if (onsec >= OFFSEC) {
      digitalWrite(LEDPIN, LOW);
      ssd1306_off();
      tickDelay = DELAY_TENTH;
      onsec = -1;
    } else {
      if (onsec >= 0) {
        digitalWrite(LEDPIN, LOW);
        bigDigital();
        tickDelay = DELAY_TENTH -70;
      }
    }
    
    if (digitalRead(BUTTON1) == LOW) {
      onsec = 0;
      ssd1306_on();
      bigDigital();
    } 

  // --------------------------------------------------
  } else if (menu==1) {
    
    if (onsec >= OFFSEC) {
      ssd1306_off();
      onsec = -1;
      menu=0;
      ohours=-1;
      ominutes=-1;
      oseconds=-1;
    }

    if (onsec==-1) onsec = 0;
    
    if (alarms==1) {
      alarms = 0;
    } else if (alarms > 1) {
      ssd1306_setpos(0,0);
      ssd1306_numdec(alarms);
      ssd1306_string_font6x8(" s");      
    }
    
  // --------------------------------------------------
  } else if (menu==2) {
    digitalWrite(LEDPIN, LOW);
    if (digitalRead(BUTTON1) == LOW) {
      hours = (hours+1)%24;
      tick=0;
      seconds=0;
      ssd1306_setpos(70,2);
      ssd1306_numdec(hours);
      ssd1306_string_font6x8("  ");
    }
    
  // --------------------------------------------------
  } else if (menu==3) {
    digitalWrite(LEDPIN, LOW);
    if (digitalRead(BUTTON1) == LOW) {
      minutes = (minutes+1)%60;
      tick=0;
      seconds=0;
      ssd1306_setpos(70,2);
      ssd1306_numdec(minutes);
      ssd1306_string_font6x8("  ");
    }
    
  // --------------------------------------------------
  } else if (menu==4) {
    digitalWrite(LEDPIN, LOW);
    if (digitalRead(BUTTON1) == LOW) {
      alarms = (alarms+10)%360;
      ssd1306_setpos(70,2);
      ssd1306_numdec(alarms);
      ssd1306_string_font6x8("  ");
    }    
  }

  if (digitalRead(BUTTON2) == LOW) {
    ssd1306_fill(0);
    menu = (menu+1)%5;
    if (menu==0) {
      // force refresh
      ohours=-1;
      ominutes=-1;
      oseconds=-1;
    } else if(menu > 1 || (menu==1 && alarms>1)) {
      ssd1306_on();
      if (menu==2) {
        readVcc();
        ssd1306_string_font6x8(" mV");
        ssd1306_setpos(0,2);
        ssd1306_string_font6x8("set hour:");
        ssd1306_setpos(70,2);
        ssd1306_numdec(hours);
        ssd1306_string_font6x8("  ");
        
      } else if (menu==3) {
        readVcc();
        ssd1306_string_font6x8(" mV");
        ssd1306_setpos(0,2);
        ssd1306_string_font6x8("set minute:");
        ssd1306_setpos(70,2);
        ssd1306_numdec(minutes);
        ssd1306_string_font6x8("  ");
      } else if (menu==4) {
        readVcc();
        ssd1306_string_font6x8(" mV");
        ssd1306_setpos(0,2);
        ssd1306_string_font6x8("set alarm:");
        ssd1306_setpos(70,2);
        ssd1306_numdec(alarms);
        ssd1306_string_font6x8("  ");
      }
    }
  }
}
