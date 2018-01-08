#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "num2str.h"
#include "num2str.c"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "Bold.h"
using namespace Bold;

// old : tested: 14h with 1MHz 65mAh Lipo

#define LEDPIN   1
#define BUTTON1  4
#define BUTTON2  3

#define OFFSEC     5
#define DELAY_QUAD 250  //should be 230 ms, but it is not (+20ms power messure delay) = 250
#define POWERMAX   3830
#define POWERMIN   3180

int hours   = 23;
int minutes = 59;
int seconds = 0;

int onsec    = 0;
byte tick    = 0;
byte ledon   = 0;

int vcc = 3700;

const byte barA1[] PROGMEM = {
  0b00111111,
  0b00001001,
  0b00000000,
  0b11110000,
  0b11110000,
  0b10000010,
  0b11111110
};

const byte barA0[] PROGMEM = {
  0b00111111,
  0b00001001,
  0b00000000,
  0b00000000,
  0b00000000,
  0b10000010,
  0b11111110
};

const byte barB2[] PROGMEM = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b11111111,
  0b11111111,
  0b10000000,
  0b11111111
};

const byte barB1[] PROGMEM = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b11111000,
  0b11111000,
  0b10000000,
  0b11111111
};

const byte barB0[] PROGMEM = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b10000000,
  0b11111111
};

const byte barC0[] PROGMEM = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b10000000,
  0b10000000,
  0b10000000,
  0b11111111
};

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

inline void bigDigital() {
  int t = hours/10;
  myFont(0, t);
  t = hours - t*10;
  myFont(17, t);
  
  t = minutes/10;
  myFont(47, t);
  t = minutes - t*10;
  myFont(64, t);

  ssd1306_setpos(93,1);
  if (seconds<10) ssd1306_numdec(0);
  ssd1306_numdec(seconds);
}

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
  ssd1306_setpos(0,0);
}


void loop() {
  ticking();
  
  if (onsec > OFFSEC) {
    ssd1306_off();
    onsec = -1;
  }
  
  if (onsec == -1 && tick==1 && seconds==1) {
    ssd1306_fill(0);
  }
  
  if (
    onsec != -1 ||                // do that stuff if display is on (not off)
    (tick==1 && ( seconds==1 || seconds==31))  // or (for a faster refresh) do it every 30 sec
  ) {  
      
    // read vcc
    power_adc_enable();
    ADMUX = (0<<REFS0) | (12<<MUX0);
    delay(20);
    ADCSRA |= (1<<ADSC); // Convert
    while (bit_is_set(ADCSRA,ADSC));
    vcc = ADCW;
    vcc = 1125300L / vcc;
    power_adc_disable(); 
  
    bigDigital();
  
    if (vcc < POWERMIN) {
      ssd1306_setpos(76,3);  
      ssd1306_numdec(vcc);
      ssd1306_string(" mV");  
    } else {
      vcc = 8.0 * ( (float)(vcc - POWERMIN) / (float)(POWERMAX - POWERMIN) );
      if (vcc > 7) {
        ssd1306_draw_bmp(120, 0,  127, 1, barA1);
        ssd1306_draw_bmp(120, 1,  127, 2, barB2);
        ssd1306_draw_bmp(120, 2,  127, 3, barB2);
        ssd1306_draw_bmp(120, 3,  127, 4, barB2);
      } else if (vcc > 6) {
        ssd1306_draw_bmp(120, 0,  127, 1, barA0);
        ssd1306_draw_bmp(120, 1,  127, 2, barB2);
        ssd1306_draw_bmp(120, 2,  127, 3, barB2);
        ssd1306_draw_bmp(120, 3,  127, 4, barB2);
      } else if (vcc > 5) {
        ssd1306_draw_bmp(120, 0,  127, 1, barA0);
        ssd1306_draw_bmp(120, 1,  127, 2, barB1);
        ssd1306_draw_bmp(120, 2,  127, 3, barB2);
        ssd1306_draw_bmp(120, 3,  127, 4, barB2);
      } else if (vcc > 4) {
        ssd1306_draw_bmp(120, 0,  127, 1, barA0);
        ssd1306_draw_bmp(120, 1,  127, 2, barB0);
        ssd1306_draw_bmp(120, 2,  127, 3, barB2);
        ssd1306_draw_bmp(120, 3,  127, 4, barB2);
      } else if (vcc > 3) {
        ssd1306_draw_bmp(120, 0,  127, 1, barA0);
        ssd1306_draw_bmp(120, 1,  127, 2, barB0);
        ssd1306_draw_bmp(120, 2,  127, 3, barB1);
        ssd1306_draw_bmp(120, 3,  127, 4, barB2);
      } else if (vcc > 2) {
        ssd1306_draw_bmp(120, 0,  127, 1, barA0);
        ssd1306_draw_bmp(120, 1,  127, 2, barB0);
        ssd1306_draw_bmp(120, 2,  127, 3, barB0);
        ssd1306_draw_bmp(120, 3,  127, 4, barB2);
      } else if (vcc > 1) {
        ssd1306_draw_bmp(120, 0,  127, 1, barA0);
        ssd1306_draw_bmp(120, 1,  127, 2, barB0);
        ssd1306_draw_bmp(120, 2,  127, 3, barB0);
        ssd1306_draw_bmp(120, 3,  127, 4, barB1);
      } else {
        ssd1306_draw_bmp(120, 0,  127, 1, barA0);
        ssd1306_draw_bmp(120, 1,  127, 2, barB0);
        ssd1306_draw_bmp(120, 2,  127, 3, barB0);
        ssd1306_draw_bmp(120, 3,  127, 4, barC0);
      }
    }
    if (ledon == 2) digitalWrite(LEDPIN, tick%2==0);

  } else {
    if (ledon == 2) {
      digitalWrite(LEDPIN, HIGH);
      delay(50);
      digitalWrite(LEDPIN, LOW);
      delay(DELAY_QUAD-50);
    } else {
      delay(DELAY_QUAD);
    }
  }

  // semi long press: hours up
  // semi long press + press additional Button2: minutes up
  if (digitalRead(BUTTON1) == LOW) {
    onsec = 0;
    ssd1306_on();
    
    delay(500);
    tick+=2;
    if (digitalRead(BUTTON1) == LOW) {
      hours = (hours+1)%24;
      seconds = 0;
    }
    while (digitalRead(BUTTON2) == LOW) {
      minutes = (minutes+1)%60;
      seconds = 0;
      bigDigital();
      delay(300);
    }
  }

  // short press: Display on
  // long press: switch LED on/off
  if (digitalRead(BUTTON2) == LOW) {
    onsec = 0;
    ssd1306_on();
    
    delay(1000);
    tick+=4;
    if (digitalRead(BUTTON2) == LOW) {
      ledon = (ledon+1)%3;
    }
    if (ledon == 1) {
      digitalWrite(LEDPIN, HIGH);
    } else if (ledon == 0) {
      digitalWrite(LEDPIN, LOW);      
    } 
  }

}
