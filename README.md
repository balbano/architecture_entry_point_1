Architecture Entry Point #1
===========================

Brendan Albano | 2015-04-02

The first in a series of research objects that explore entry points to hook
into architectural systems and modify their behavior.

Connections
-----------

### Power

Power the arduino by running 5v IN to the 5v pin. This is not how you are
"supposed" to power the Arduino, but any other method of powering it was
resulting in the Bluetooth connection resetting itself every time the
servo activated.

### Servo

- Signal: Pin 3
- Power: 5v
- Ground: Ground

### Adafruit Bluefruit LE

- VIN: 5v
- GND: Ground
- SCK: Pin 13
- MISO: Pin 12
- MOSI: Pin 11
- REQ: Pin 10
- RST: Pin 9
- RDY: Pin 2

### Adafruit Data Logger Shield

- It's a shield: plug it in!
- It uses pins A4 and A5 for I2C

I'm just using this shield for the real-time clock because I already had it lying
around. You can use any real-time clock.

Libraries to download
---------------------

- DS1307RTC https://github.com/PaulStoffregen/DS1307RTC
- Time https://github.com/PaulStoffregen/Time
- Adafruit_nRF8001 https://github.com/adafruit/Adafruit_nRF8001

License
-------

The MIT License (MIT)

Copyright (c) 2015 Brendan Albano

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
