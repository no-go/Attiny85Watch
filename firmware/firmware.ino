#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI




#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "ssd1306xled.h"
#include "font6x8.h"
#include "num2str.h"

// ----------------------------------------------------------------------------

// Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358

const uint8_t ssd1306_init_sequence [] PROGMEM = {  // Initialization Sequence
  0xAE,     // Display OFF (sleep mode)
  0x20, 0b00,   // Set Memory Addressing Mode
          // 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
          // 10=Page Addressing Mode (RESET); 11=Invalid
  0xB0,     // Set Page Start Address for Page Addressing Mode, 0-7
  0xC8,     // Set COM Output Scan Direction
  0x00,     // ---set low column address
  0x10,     // ---set high column address
  0x40,     // --set start line address
  0x81, 0x8F,   // Set contrast control register
  0xA1,     // Set Segment Re-map. A0=address mapped; A1=address 127 mapped. 
  0xA6,     // Set display mode. A6=Normal; A7=Inverse
//  0xA8, 0x3F,   // Set multiplex ratio(1 to 64)
  0xA8, 0x1F,   // Set multiplex ratio(1 to 32)
  0xA4,     // Output RAM to Display
          // 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
  0xD3, 0x00,   // Set display offset. 00 = no offset
  0xD5,     // --set display clock divide ratio/oscillator frequency
  0x80,     // --set divide ratio
  0xD9, 0x22,   // Set pre-charge period
  0xDA, 0x02,   // Set com pins hardware configuration    
  0xDB,     // --set vcomh
  0x20,     // 0x20,0.77xVcc
  0x8D, 0x14,   // Set DC-DC enable
  0xAF      // Display ON in normal mode
  
};

void ssd1306_send_command(uint8_t command) {
  TinyWireM.beginTransmission(SSD1306_SA);
  TinyWireM.send(command);
  TinyWireM.endTransmission();
}

void ssd1306_send_data_start(void) {
  TinyWireM.beginTransmission(SSD1306_SA);
  TinyWireM.send(0x40);  //write data
}

void ssd1306_init(void) { 
  for (uint8_t i = 0; i < sizeof (ssd1306_init_sequence); i++) {
    ssd1306_send_command(pgm_read_byte(&ssd1306_init_sequence[i]));
  }
}

void ssd1306_setpos(uint8_t x, uint8_t y) {
  TinyWireM.beginTransmission(SSD1306_SA);
  TinyWireM.send(0xb0 + y);
  TinyWireM.send(((x & 0xf0) >> 4) | 0x10); // | 0x10
/* TODO: Verify correctness */  TinyWireM.send((x & 0x0f)); // | 0x01
  TinyWireM.endTransmission();
}

void ssd1306_fill4(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4) {
  ssd1306_setpos(0, 0);
  TinyWireM.beginTransmission(SSD1306_SA);
  for (uint16_t i = 0; i < 128 * 8 / 4; i++) {
    TinyWireM.send(p1);
    TinyWireM.send(p2);
    TinyWireM.send(p3);
    TinyWireM.send(p4);
  }
  TinyWireM.endTransmission();
}

void ssd1306_fill2(uint8_t p1, uint8_t p2) {
  ssd1306_fill4(p1, p2, p1, p2);
}

void ssd1306_fill(uint8_t p) {
  ssd1306_fill4(p, p, p, p);
}

void ssd1306_char_font6x8(char ch) {
  uint8_t c = ch - 32;
  ssd1306_send_data_start();
  for (uint8_t i = 0; i < 6; i++) {
    TinyWireM.send(pgm_read_byte(&ssd1306xled_font6x8[c * 6 + i]));
  }
  TinyWireM.endTransmission();
}

void ssd1306_string_font6x8(char *s) {
  while (*s) {
    ssd1306_char_font6x8(*s++);
  }
}

char ssd1306_numdec_buffer[USINT2DECASCII_MAX_DIGITS + 1];

void ssd1306_numdec_font6x8(uint16_t num) {
  ssd1306_numdec_buffer[USINT2DECASCII_MAX_DIGITS] = '\0';   // Terminate the string.
  uint8_t digits = usint2decascii(num, ssd1306_numdec_buffer);
  ssd1306_string_font6x8(ssd1306_numdec_buffer + digits);
}

void ssd1306_numdecp_font6x8(uint16_t num) {
  ssd1306_numdec_buffer[USINT2DECASCII_MAX_DIGITS] = '\0';   // Terminate the string.
  usint2decascii(num, ssd1306_numdec_buffer);
  ssd1306_string_font6x8(ssd1306_numdec_buffer);
}

// ----------------------------------------------------------------------------

void ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]) {
  uint16_t j = 0;
  uint8_t y;
  if (y1 % 8 == 0) y = y1 / 8;
  else y = y1 / 8 + 1;
  for (y = y0; y < y1; y++)
  {
    ssd1306_setpos(x0,y);
    ssd1306_send_data_start();
    for (uint8_t x = x0; x < x1; ++x) {
      TinyWireM.send(pgm_read_byte(&bitmap[j++]));
    }
    TinyWireM.endTransmission();
  }
}


void ssd1306_off(void) {
  ssd1306_send_command(0xAE);
}

void ssd1306_on(void) {
  ssd1306_send_command(0xAF);  
}

















#include "num2str.h"
#include "num2str.c"

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

#define MAG_ADDR 0x1E                   // 7 bit I2C address (PB2 = SCL, PB0 = SDA)
int mx,my,mz;

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
  delay(tickDelay);
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

void mag_read() {
  TinyWireM.beginTransmission(MAG_ADDR);
  // Tell the HMC5883L where to begin reading data
  // with register 3 (the X MSB register)
  TinyWireM.send(0x03);
  TinyWireM.endTransmission();
  ticking();
  //Read data from each axis, 2 registers per axis
  TinyWireM.requestFrom(MAG_ADDR, 6);
  if(6<=TinyWireM.available()){
    mx = TinyWireM.receive()<<8; //X msb
    mx |= TinyWireM.receive(); //X lsb
    mz = TinyWireM.receive()<<8; //Z msb
    mz |= TinyWireM.receive(); //Z lsb
    my = TinyWireM.receive()<<8; //Y msb
    my |= TinyWireM.receive(); //Y lsb
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

  TinyWireM.begin();
  
  TinyWireM.beginTransmission(MAG_ADDR);
  TinyWireM.send(0x02); //select mode register
  TinyWireM.send(0x00); //continuous measurement mode
  TinyWireM.endTransmission();
  
  ssd1306_init();
  ssd1306_fill(0);
  delay(500);
}


void loop() {
  jumy12:
  ticking();

  mag_read();
  if (mx>800 || digitalRead(BUTTON2) == LOW) {
    digitalWrite(LEDPIN, HIGH);
  } else {
    digitalWrite(LEDPIN, LOW);
  }

  goto jumy12;
  
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
      ticking();
    }    
    if (tick==1) {
      mag_read();
      ssd1306_setpos(10,1);
      ssd1306_numdec(mx);
      
      ssd1306_setpos(70,2);
      ssd1306_numdec(alarms);
      ssd1306_string_font6x8("  ");
    }
  }

  if (digitalRead(BUTTON2) == LOW) {
    ssd1306_fill(0);
    tickDelay = DELAY_TENTH;
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
