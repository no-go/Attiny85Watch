#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "num2str.h"
#include "num2str.c"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "Bold.h"
#include "DotNumbers.h"
#include "Normal.h"
#include "FuturNumbers.h"

#include "Hglas.h"

// font
using namespace Normal;

#define LEDPIN   1
#define BUTTON1  3
#define BUTTON2  4

#define OFFSEC  5
#define POWERWARN 3640

int hours   = 0;
int minutes = 0;
int seconds = 0;
int alarms =  0;

int onsec    = 0;
byte tick    = 0;
byte ledon   = 0;
int vcc      = 3800;

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
  }
}

inline void readVcc() {
  // read vcc
  power_adc_enable();
  ADMUX = (0<<REFS0) | (12<<MUX0);
  delay(10);
  ADCSRA |= (1<<ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  vcc = ADCW;
  vcc = 1125300L / vcc;
  power_adc_disable();
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
    if (vcc < POWERWARN) {
      if (tick%2==0) {
        ssd1306_draw_bmp(111, 0, 127, 8, Hglas::bat);        
      } else {
        ssd1306_draw_bmp(111, 0, 127, 8, Hglas::nothing);
      }
    } else {
      myFont2(111, tick);
    }
  }
}

void setup() {
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(LEDPIN,  OUTPUT);
  sleep_bod_disable();
  power_adc_disable();
  power_timer1_disable();
  
  ssd1306_init();
  ssd1306_fill(0);
  delay(500);
  
  MCUSR = MCUSR & B11110111; // Reset flag disable, WDRF bit3 of MCUSR
  WDTCR = WDTCR | B00011000; // set Bit 3+4 to set Prescaler
  WDTCR = B00000011; // Watchdog Prescaler auf 16k setzen > ergibt ca. 120 ms
  WDTCR = WDTCR | B01000000; // wdt enable

  //MCUSR = MCUSR & B11110111;
//  ADCSRA = ADCSRA & B01111111; // ADC abschalten, ADEN bit7 zu 0
//  ACSR = B10000000; // Analogen Comparator abschalten, ACD bit7 zu 1
//  DIDR0 = DIDR0 | B00111111; // Digitale Eingangspuffer ausschalten, analoge Eingangs Pins 0-5 auf 1
}


void loop() {

  if (alarms==1 && tick==3) {
    delay(125);
  } else {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
    sleep_disable();    
  }
    
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
      readVcc();
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
      ssd1306_string_font6x8(" s   ");      
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
      alarms = (((alarms/10)+1)%36)*10;
      ssd1306_setpos(70,2);
      ssd1306_numdec(alarms);
      ssd1306_string_font6x8("  ");
      // delay(125)
    }    
    if (tick==1) {
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
        ssd1306_setpos(0,0);  
        ssd1306_numdec(vcc); 
        ssd1306_string_font6x8(" mV");
        ssd1306_setpos(0,2);
        ssd1306_string_font6x8("set hour:");
        ssd1306_setpos(70,2);
        ssd1306_numdec(hours);
        ssd1306_string_font6x8("  ");
        
      } else if (menu==3) {
        readVcc();
        ssd1306_setpos(0,0);  
        ssd1306_numdec(vcc); 
        ssd1306_string_font6x8(" mV");
        ssd1306_setpos(0,2);
        ssd1306_string_font6x8("set minute:");
        ssd1306_setpos(70,2);
        ssd1306_numdec(minutes);
        ssd1306_string_font6x8("  ");
      } else if (menu==4) {
        readVcc();
        ssd1306_setpos(0,0);  
        ssd1306_numdec(vcc); 
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

ISR(WDT_vect) {
  tick++;
  // fix speed (1/16 sec faster)
  // but not every 8sec (because it was 1sec every min to fast)
  // but we lost 1min in 3h: fix it with adding 1/3 every 60s (similar to 1/8 ever 20sec = 3times in 1minute)
  if ( (tick==6) &&
    // Verschlucke 1/8 sec jede gerade sekunde, es sei denn, sie ist durch 8 teilbar und nicht 18, 36 oder 56 
    ((seconds%2)==0) && 
    ((seconds%8)>0) && 
    ( (seconds!=18) || (seconds!=36) )
  ) {
    tick=7;
  }
  
  if (tick > 7) {
    seconds += tick/8;
    if (onsec>=0 && onsec <= OFFSEC) onsec++;
  }
  
  if (tick > 7) {
    if (alarms > 1) alarms--;
    tick = tick % 8;
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
