#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "ssd1306xled8x16.h"
#include "ssd1306xled8x16.c"
#include "myNumbers.h"
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

int vcc = 3900;


void myFont2(byte x, short y, byte b) {
  if (b == 0) {
    ssd1306_string_font8x16xy(x, y, "0");
  } else if (b == 1) {
    ssd1306_string_font8x16xy(x, y, "1");
  } else if (b == 2) {
    ssd1306_string_font8x16xy(x, y, "2");
  } else if (b == 3) {
    ssd1306_string_font8x16xy(x, y, "3");
  } else if (b == 4) {
    ssd1306_string_font8x16xy(x, y, "4");
  } else if (b == 5) {
    ssd1306_string_font8x16xy(x, y, "5");
  } else if (b == 6) {
    ssd1306_string_font8x16xy(x, y, "6");
  } else if (b == 7) {
    ssd1306_string_font8x16xy(x, y, "7");
  } else if (b == 8) {
    ssd1306_string_font8x16xy(x, y, "8");
  } else if (b == 9) {
    ssd1306_string_font8x16xy(x, y, "9");
  }
}

void myFont(byte x, short y, byte b) {
  if (b == 0) {
    ssd1306_draw_bmp(x, y, 16, 32, zz0);
  } else if (b == 1) {
    ssd1306_draw_bmp(x, y, 16, 32, zz1);
  } else if (b == 2) {
    ssd1306_draw_bmp(x, y, 16, 32, zz2);
  } else if (b == 3) {
    ssd1306_draw_bmp(x, y, 16, 32, zz3);
  } else if (b == 4) {
    ssd1306_draw_bmp(x, y, 16, 32, zz4);
  } else if (b == 5) {
    ssd1306_draw_bmp(x, y, 16, 32, zz5);
  } else if (b == 6) {
    ssd1306_draw_bmp(x, y, 16, 32, zz6);
  } else if (b == 7) {
    ssd1306_draw_bmp(x, y, 16, 32, zz7);
  } else if (b == 8) {
    ssd1306_draw_bmp(x, y, 16, 32, zz8);
  } else if (b == 9) {
    ssd1306_draw_bmp(x, y, 16, 32, zz9);
  }
}

inline void bigDigital() {
  int t = hours/10;
  myFont(0, 0,t);
  t = hours - t*10;
  myFont(16, 0,t);
  
  t = minutes/10;
  myFont(25, 0,t);
  t = minutes - t*10;
  myFont(41, 0,t);

  t = seconds/10;
  myFont(50, 0,t);
  t = seconds - t*10;
  myFont(66, 0,t);
}

inline void bigDigital2() {
  int t = hours/10;
  myFont2(0, 0,t);
  t = hours - t*10;
  myFont2(8, 0,t);
  
  t = minutes/10;
  myFont2(22, 0,t);
  t = minutes - t*10;
  myFont2(30, 0,t);

  t = seconds/10;
  myFont2(44, 0,t);
  t = seconds - t*10;
  myFont2(52, 0,t);
}

void readVcc() {
  // Read 1.1V reference against Vcc
  ADMUX = (0<<REFS0) | (12<<MUX0);
  delay(2);
  ADCSRA |= (1<<ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  vcc = ADCW;
  vcc = 1125300L / vcc; 
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
  delay(246);
  readVcc();

  bigDigital2();
  
  ssd1306_setpos(0,3);
  ssd1306_numdec(vcc);  
  ssd1306_string(" mV");
  
  //ssd1306_draw(0, 0, 16, 32, zz0);
  
  /*
  ssd1306_setpos(20,1);
  if (hours < 10) ssd1306_char('0');
  ssd1306_numdec(hours);
  ssd1306_char(':');
  if (minutes < 10) ssd1306_char('0');
  ssd1306_numdec(minutes);
  ssd1306_char(':');
  if (seconds < 10) ssd1306_char('0');
  ssd1306_numdec(seconds);
  */
  
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
    delay(500);
    tick+=2;
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
