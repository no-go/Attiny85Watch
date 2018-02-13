#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "num2str.h"
#include "num2str.c"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include "FuturNumbers.h"
using namespace FuturNumbers;

// old : tested: 14h with 1MHz 65mAh Lipo

#define LEDPIN   1
#define BUTTON1  4
#define BUTTON2  3

#define OFFSEC     5
#define DELAY_QUAD 250  //should be 230 ms, but it is not (+20ms power messure delay) = 250
#define POWERMAX   4130
#define POWERMIN   3620

int hours   = 0;
int minutes = 0;
int seconds = 0;

int onsec    = 0;
byte tick    = 0;
byte ledon   = 0;

int vcc = 3730;

const byte barA1[] PROGMEM = {
  0b11111110,
  0b11111110
};

const byte barA2[] PROGMEM = {
  0b11110000,
  0b11110000
};

const byte barA3[] PROGMEM = {
  0b00000000,
  0b00000000
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

void scroll() {
  ssd1306_init();
  ssd1306_send_command(0x27); //SSD1306_LEFT_HORIZONTAL_SCROLL
  ssd1306_send_command(0X00);
  ssd1306_send_command(0x00); // start row
  ssd1306_send_command(0X00);
  ssd1306_send_command(3); // stop row
  ssd1306_send_command(0X00);
  ssd1306_send_command(0XFF);
  ssd1306_send_command(0x2F); //SSD1306_ACTIVATE_SCROLL  
}

inline void bigDigital() {
  // read vcc
  power_adc_enable();
  ADMUX = (0<<REFS0) | (12<<MUX0);
  ssd1306_fill(0); // is a delay
  ADCSRA |= (1<<ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  vcc = ADCW;
  vcc = 1125300L / vcc;
  power_adc_disable();

    
  int t = hours/10;
  myFont(4, t);
  t = hours - t*10;
  myFont(20, t);
  
  t = minutes/10;
  myFont(40, t);
  t = minutes - t*10;
  myFont(56, t);

  t = seconds/10;
  myFont(92, t);
  t = seconds - t*10;
  myFont(108, t);



  if (vcc < POWERMIN) {
    ssd1306_setpos(0,0);  
    ssd1306_numdec(vcc); 
  } else {
    vcc = 8.0 * ( (float)(vcc - POWERMIN) / (float)(POWERMAX - POWERMIN) );
    if (vcc > 7) {
      ssd1306_draw_bmp(125, 0,  127, 1, barA1);
      ssd1306_draw_bmp(125, 1,  127, 2, barA1);
      ssd1306_draw_bmp(125, 2,  127, 3, barA1);
      ssd1306_draw_bmp(125, 3,  127, 4, barA1);
    } else if (vcc > 6) {
      ssd1306_draw_bmp(125, 0,  127, 1, barA2);
      ssd1306_draw_bmp(125, 1,  127, 2, barA1);
      ssd1306_draw_bmp(125, 2,  127, 3, barA1);
      ssd1306_draw_bmp(125, 3,  127, 4, barA1);
    } else if (vcc > 5) {
      ssd1306_draw_bmp(125, 0,  127, 1, barA3);
      ssd1306_draw_bmp(125, 1,  127, 2, barA1);
      ssd1306_draw_bmp(125, 2,  127, 3, barA1);
      ssd1306_draw_bmp(125, 3,  127, 4, barA1);
    } else if (vcc > 4) {
      ssd1306_draw_bmp(125, 0,  127, 1, barA3);
      ssd1306_draw_bmp(125, 1,  127, 2, barA2);
      ssd1306_draw_bmp(125, 2,  127, 3, barA1);
      ssd1306_draw_bmp(125, 3,  127, 4, barA1);
    } else if (vcc > 3) {
      ssd1306_draw_bmp(125, 0,  127, 1, barA3);
      ssd1306_draw_bmp(125, 1,  127, 2, barA3);
      ssd1306_draw_bmp(125, 2,  127, 3, barA1);
      ssd1306_draw_bmp(125, 3,  127, 4, barA1);
    } else if (vcc > 2) {
      ssd1306_draw_bmp(125, 0,  127, 1, barA3);
      ssd1306_draw_bmp(125, 1,  127, 2, barA3);
      ssd1306_draw_bmp(125, 2,  127, 3, barA2);
      ssd1306_draw_bmp(125, 3,  127, 4, barA1);
    } else if (vcc > 1) {
      ssd1306_draw_bmp(125, 0,  127, 1, barA3);
      ssd1306_draw_bmp(125, 1,  127, 2, barA3);
      ssd1306_draw_bmp(125, 2,  127, 3, barA3);
      ssd1306_draw_bmp(125, 3,  127, 4, barA1);
    } else {
      ssd1306_draw_bmp(125, 0,  127, 1, barA3);
      ssd1306_draw_bmp(125, 1,  127, 2, barA3);
      ssd1306_draw_bmp(125, 2,  127, 3, barA3);
      ssd1306_draw_bmp(125, 3,  127, 4, barA2);
    }
  }
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
  
  ssd1306_fill(0);
  delay(500);
  ssd1306_setpos(0,0);
}


void loop() {
  ticking();
  
  if (onsec > OFFSEC) {
    ssd1306_off();
    ssd1306_send_command(0x2E); // SSD1306_DEACTIVATE_SCROLL
    onsec = -1;
  }
  
  if (ledon == 2) {
    digitalWrite(LEDPIN, HIGH);
    delay(50);
    digitalWrite(LEDPIN, LOW);
    delay(DELAY_QUAD-50);
  } else {
    delay(DELAY_QUAD);
  }

  // semi long press: hours up
  // semi long press + press additional Button2: minutes up
  if (digitalRead(BUTTON1) == LOW) {
    onsec = 0;
    ssd1306_on();
    ssd1306_send_command(0x2E);
    bigDigital();
    scroll();
    
    delay(500);
    tick+=2;
    
    if (digitalRead(BUTTON1) == LOW) ssd1306_send_command(0x2E);
    while (digitalRead(BUTTON1) == LOW) {
      hours = (hours+1)%24;
      seconds = 0;
      bigDigital();
      delay(200);
    }
    
    if (digitalRead(BUTTON2) == LOW) ssd1306_send_command(0x2E);
    while (digitalRead(BUTTON2) == LOW) {
      minutes = (minutes+1)%60;
      seconds = 0;
      bigDigital();
      delay(200);
    }
  }

  // short press: Display on
  // long press: switch LED on/off
  if (digitalRead(BUTTON2) == LOW) {
    onsec = 0;
    ssd1306_on();
    ssd1306_send_command(0x2E);
    bigDigital();
    scroll();
    
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
