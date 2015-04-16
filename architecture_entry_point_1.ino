/*
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

Aditional notes
---------------
- Writing to the BTLESerial when nothing is connected appears to stall the program. Avoid doing that!
- Sometimes the weight of the flicker arm causes the servo to make obnoxious noises when at rest. 
  Try detaching it or cuttin power with a transistor.

*/

//Servo includes
#include <Servo.h>

// Bluefruit LE includes
#include <SPI.h>
#include "Adafruit_BLE_UART.h"

// RTC includes
#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>

// Servo setup.
Servo switch_flicker;
const int rest_position = 150;
const int switch_position = 112;
const int servo_pin = 3;

// Adafruit Bluefruit LE setup.
// Connect CLK/MISO/MOSI to hardware SPI
// e.g. On UNO & compatible: CLK = 13, MISO = 12, MOSI = 11
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2     // This should be an interrupt pin, on Uno thats #2 or #3
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

// Holds the serial message.
String message = "";
int alarm_hour = 6;
int alarm_minute = 30;
time_t switch_last_flicked = 0;

void setup() {
  // Servo setup.
  switch_flicker.attach(servo_pin);
  switch_flicker.write(rest_position);
  
  // Serial setup.
  Serial.begin(9600);

  // Bluefruit setup
  BTLEserial.setDeviceName("Switchr"); /* 7 characters max! */
  BTLEserial.begin();
  
  // RTC setup
  Wire.begin();
  while (!Serial) ; // wait until Arduino Serial Monitor opens
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     Serial.println(F("Unable to sync with the RTC"));
  else
     Serial.println(F("RTC has set the system time"));    
}

aci_evt_opcode_t laststatus = ACI_EVT_DISCONNECTED;

void loop() {
  // Check the alarm
  time_t current_time = now();
  if (time_to_flick(current_time)) {
    flick_switch();
    switch_last_flicked = current_time;
  }
  
  // Tell the nRF8001 to do whatever it should be working on.
  BTLEserial.pollACI();

  // Ask what is our current status
  aci_evt_opcode_t status = BTLEserial.getState();
  // If the status changed....

  if (status != laststatus) {
    // print it out!
    print_status(status);
    
    // OK set the last status change to this one
    laststatus = status;
  }

  if (status == ACI_EVT_CONNECTED) {
    // Lets see if there's any data for us!
    listen_for_command();
  }
}

// Parses serial input over Bluetooth LE connection.
void listen_for_command() {
  while (BTLEserial.available() > 0) {
    char character = BTLEserial.read();

    if (character != '\n') {
      message += character;
    }
    else {
      if (message == "flick") {
        flick_switch();
      }
      else if (message == "time") {
        BTLEserial.print(F("Current time: "));
        BTLEserial.print(time_to_string(now()).c_str());
        BTLEserial.print(F("\n"));
      }
      else if (message == "alarm") {
        BTLEserial.print(F("Alarm set to: "));
        BTLEserial.print(alarm_hour);
        BTLEserial.print(F(":"));
        BTLEserial.print(alarm_minute);
        BTLEserial.print(F("\n"));
      }
      else if (message.length() == 8 
               && message.substring(0, 3) == "set"
               && is_all_digits(message.substring(4))) {
        alarm_hour = message.substring(4,6).toInt();
        alarm_minute = message.substring(6).toInt();
        BTLEserial.print(F("Alarm set to: "));
        BTLEserial.print(alarm_hour);
        BTLEserial.print(F(":"));
        BTLEserial.print(alarm_minute);
        BTLEserial.print(F("\n"));
      }
      else {
        BTLEserial.print(F("You typed: '"));
        BTLEserial.print(message);
        BTLEserial.print(F("'\n"));
        BTLEserial.print(F("I don't know what that means.\n"));
        BTLEserial.print(F("Type 'set hhmm' to set the alarm.\n"));
      }

      message = "";
    }
  }
}

void flick_switch() {
  switch_flicker.write(rest_position);
  delay(500);
  switch_flicker.write(switch_position);
  delay(500);
  switch_flicker.write(rest_position);
  delay(500);
}

// Check to see if it is the alarm time and that the switch hasn't been flicked.
boolean time_to_flick(time_t current_time){
  return (hour(current_time) == alarm_hour 
          && minute(current_time) == alarm_minute
          && switch_not_flicked_this_minute(current_time));
}

boolean switch_not_flicked_this_minute(time_t current_time) {
  return !(day(current_time) == day(switch_last_flicked)
           && hour(current_time) == hour(switch_last_flicked)
           && minute(current_time) == minute(switch_last_flicked));
}

// String and printing utilities
boolean is_all_digits(String str) {
  boolean result = true;
  for (int i = 0; i < str.length(); ++i){
    result = (result && isdigit(str[i]));
  }
  return result;
}

String time_to_string(time_t current_time) {
  return String(year(current_time))
    + '/'
    + String(month(current_time))
    + '/'
    + String(day(current_time))
    + ' '
    + String(hour(current_time))
    + ':'
    + String(minute(current_time))
    + ':'
    + String(second(current_time));
}

void print_status(aci_evt_opcode_t status) {
  if (status == ACI_EVT_DEVICE_STARTED) {
    Serial.println(F("* Advertising started"));
  }
  if (status == ACI_EVT_CONNECTED) {
    Serial.println(F("* Connected!"));
  }
  if (status == ACI_EVT_DISCONNECTED) {
    Serial.println(F("* Disconnected or advertising timed out"));
  }
}

