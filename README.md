# Attiny85Watch

attiny85 with white LED and 128x32 i2c oled display - 2 buttons for set hours and minutes

# Circuit

![Circuit](img/circuit.jpg)

# done

 -  set to 1Mhz to save power
 -  set hours/minutes (hours Button 1, minutes Button1 + 2)
 -  switch LED on/off (long press Button 2)
 -  button on / automatic off
 -  power in % extrem values in mV
 -  test about time accurency (need 148ms for 250ms - that is strange)
 -  test lipo life time (tested for 9h)

# future steps

 -  power bar
 -  big and flapping numbers
 -  nice circuit / more details
 -  programming how-to
 -  watchcase, chassis, design

# Strange timing and flashing

I have to switch via `16MHz.sh` skript from 1MHz back to 16MHz, because Arduino IDE
can not flash ATtiny85 in 1MHz mode. After programming I have to switch ATtiny85 back with
the `1MHz.sh` script.

# Thanks to ..

Thanks to Neven Boyanov and the Tinusaur/SSD1306xLED project. (MIT License)
The [Tinusaur Project](http://tinusaur.org). I modified it to use a
128x32 instead of 128x64 oled display.
