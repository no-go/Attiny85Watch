# ATtiny85 Watch

Use a ATtiny85 8pin AVR chip, a white LED, a 128x32 i2c oled display and 2 buttons for set hours and minutes as watch!

<a href="http://www.youtube.com/watch?feature=player_embedded&v=GaI7kfXpqJI" target="_blank"><img src="http://img.youtube.com/vi/GaI7kfXpqJI/0.jpg" 
alt="Video" width="240" height="180" border="10" /></a>

[Project page](https://no-go.github.io/Attiny85Watch/)

[Jochen Peters Krefeld](http://digisocken.de/blog.html)

This Branch:

 -  optimized for 1MHz (8Mhz needs more power)
 -  lifetime: more than 15h on 1MHz (65mAh Lipo after charging it)
 -  displays the Lipo power in mV (menu mode)
 -  Button 1: Display on (clock mode) or set hour/minutes
 -  Button 2: switch through clock, led on, menu mode (set hours), menu mode (set minutes)
 -  time accurency: after more than 8h 3min+
 -  10th of a second: a hour glas

# Circuit

I buy a 20 Dollar headset and use the small 65mAh lipo to shrink everything!

![Circuit ATtiny85 Watch](img/circuit.jpg)

# Hardware development images

![easy wired](img/backside.jpg)
![tiny size](img/tiny.jpg)

# Hint 1MHz and 8MHz

Read and use the `8MHz.sh` and `1MHz.sh` Linux shell script to set ATtiny85 fuse bits.
My Arduino IDE is unable to flash the attiny85 with 1MHz, if the fuse bits are set to
1MHz. After flashing the code, use `1MHz.sh` to set the attiny fuse bits back to 1MHz.

# Thanks to ..

Thanks to Neven Boyanov and the Tinusaur/SSD1306xLED project. (MIT License)
The [Tinusaur Project](http://tinusaur.org). I modified it to use a
128x32 instead of 128x64 oled display.
