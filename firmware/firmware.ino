#include "ssd1306xled.h"
#include "ssd1306xled.c"
#include "ssd1306xled8x16.h"
#include "ssd1306xled8x16.c"
#include "num2str.h"
#include "num2str.c"

#define LEDPIN   1
#define BUTTON1  4
#define BUTTON2  3

#define OFFSEC   5

#define REAL250msDELAY 155

#define POWERMAX 3950
#define POWERMIN 3200

int hours   = 0;
int minutes = 0;
int seconds = 0;
int onsec   = 0;
byte tick    = 0;
bool ledon = false;

int vcc = 3700;


void myFont(byte x, short y, byte b) {
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

inline void bigDigital() {
  int t = hours/10;
  myFont(0, 0,t);
  t = hours - t*10;
  myFont(8, 0,t);
  
  t = minutes/10;
  myFont(22, 0,t);
  t = minutes - t*10;
  myFont(30, 0,t);

  t = seconds/10;
  myFont(44, 0,t);
  t = seconds - t*10;
  myFont(52, 0,t);
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
  
  ssd1306_init();
  ssd1306_fill(0);
  delay(500);
  ssd1306_setpos(0,0);
}

void loop() {
  ticking();

  if (onsec != -1) {  
    delay(REAL250msDELAY -10);
  
    // read vcc
    ADMUX = (0<<REFS0) | (12<<MUX0);
    delay(10);
    ADCSRA |= (1<<ADSC); // Convert
    while (bit_is_set(ADCSRA,ADSC));
    vcc = ADCW;
    vcc = 1125300L / vcc; 
  
    bigDigital();
  
    if (vcc > POWERMAX || vcc < POWERMIN) {
      ssd1306_setpos(0,3);
      ssd1306_numdec(vcc);  
      ssd1306_string(" mV");  
    } else {
      vcc = 100.0 * ( (float)(vcc - POWERMIN) / (float)(POWERMAX - POWERMIN) );
      ssd1306_setpos(0,3);
      ssd1306_numdec(vcc);  
      ssd1306_string(" %   ");
    }
  } else {
    delay(250);
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
      ledon = !ledon;
      if (ledon == true) {
        digitalWrite(LEDPIN, HIGH);
      } else {
        digitalWrite(LEDPIN, LOW);      
      }
    }
  }

  if (onsec > OFFSEC) {
    ssd1306_off();
    onsec = -1;
  }
}
