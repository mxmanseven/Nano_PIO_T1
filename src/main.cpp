
/*
This is an American Motorcycle Association Enduro computer.
Given a route of speed averages, resets, free time, and known controls
this computer will give the user their pace, distance, and time,
all the information needed to win!

   Hardware setup:
Microcontroller:
   Arduino Nano 238P
RTC / EEPROM:
   ZS-042 break out board
   DS3231  RTC
   AT24C32 32kbit eeprom

Connections:
   ZS-042 is connected to the arduino with i2c.
   IIC sac and sad lines are on pins a4 and a5
   it is powered with 5v from the teensy
   External pull up resistors are used.
   I think they are 5k.

    LCD
    The remaining analog pins are used to drive the LCD.

    Buttons:
    Four buttons with hardware debouncing are on pins d2 - d5.
    That is port D
    74hc14 schmit trigger is used with an RC filter handel the debouncing.

    Wheel Sensor:
    d8
    Port B
*/

#include <Arduino.h>
#include <avr/interrupt.h>

#include "display.h"
#include "WheelManager.h"
#include "SpeedInterval.h"

#include <Wire.h> //https://forum.pjrc.com/threads/21680-New-I2C-library-for-Teensy3
//#include <i2c_t3.h>
// #include <hd44780.h>                       // main hd44780 header
// #include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

#include <LiquidCrystal.h>

#include "TimeService.h"
#include "EepromIic.h"
#include "Route.h"
#include "EnduroManager.h"
#include "Buttons.h"

//hd44780_I2Cexp lcd; // declare lcd object: auto locate & config exapander chip

TimeServicKnh timeKnh;
WheelManager wm;
EepromIic eepromIic;
EnduroManager em;
Buttons buttons;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = PIN_A0, en = PIN_A1, d4 = PIN_A2, d5 = PIN_A3, d6 = PIN_A6, d7 = PIN_A7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

volatile int pushed = 0;
volatile int buttonIntEventRaised = 0;

ISR (PCINT2_vect)
{
    // PCINT2_vect -> port d -> pins d2 - d5.
    // toggle LED
    int value = ++pushed % 2;
    buttonIntEventRaised = 1;
    digitalWrite(LED_BUILTIN, value);
}

void setup() 
{
    // //   // set up the LCD's number of columns and rows:
    // lcd.begin(20, 4);

    // lcd.write("hi!");

         // wait for a second
  digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
  delay(200);
  digitalWrite(9, HIGH);                  
  delay(200);
  digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
  delay(200);
  digitalWrite(9, HIGH);                  
  delay(200);

    Serial.begin(19200);
    while(!Serial) {}
    Wire.begin();

    delay(100);

    buttons.Init(2, 3, 4, 5);
    buttons.SetupPins();
    
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  //end blink 
}

float tenthMilesToPossiable = 0;
int16_t secondsOffPace = 0;

uint32_t i = 0;

void loop()
{


//blink
  digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(10000);                       // wait for a second
  digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
  delay(10000);                       // wait for a second

  // end blink

    Serial.println("hi");
    if(buttonIntEventRaised > 0)
    {
        Serial.println("button read form main");

        buttonIntEventRaised = 0;

        buttons.ReadButtons();

        
        for(int i =  0; i < Buttons::PIN_COUNT; i++)
        {
            buttons.pins[i].pressedLong = false;
            buttons.pins[i].pressedShort = false;
        }
    }





    // if(i++ == 0) em.startEnduro();

    // uint32_t microSecondsStart = millis();

    // wm.AddTickRaw();

    // em.getRaceData( 
    //     tenthMilesToPossiable,
    //     secondsOffPace);

    // float distance = wm.GetTotalDistance();

    // uint32_t durationMicroSeconmds = millis() - microSecondsStart;
    
    // Serial.print(F("durationMicroSeconmds: "));
    // Serial.println(durationMicroSeconmds);

    // Serial.print(F("getRaceSeconds: "));
    // Serial.println(timeKnh.getRaceSeconds());

    // Serial.print(F("distance: "));
    // Serial.println(distance);

    // Serial.print(F("speed: "));
    // Serial.println(wm.GetSpeed(2));

    // Serial.print(F("tenthMilesToPossiable: "));
    // Serial.println(tenthMilesToPossiable);

    // Serial.print(F("secondsOffPace: "));
    // Serial.println(secondsOffPace);

    // Serial.println("");
}