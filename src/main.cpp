

#include <avr/sleep.h>
#include <Arduino.h>
#include <avr/interrupt.h>

volatile int pushed = 0;
const byte LEDWAKE = 13;


// four buttons on pins 2, 3, 4, 5
// that is port D.
const uint8_t BUTTON_PIN_COUNT = 4;
uint8_t buttonPins[BUTTON_PIN_COUNT] = {2, 3, 4, 5};


void SetupButtonPins()
{
    for(int i =  0; i < BUTTON_PIN_COUNT; i++)
    {
        uint8_t pin = buttonPins[i];   
        pinMode(pin, INPUT);
        digitalWrite (pin, HIGH);  // enable pull-up  
        Serial.print("Set Pin as high input: ");
        Serial.println(String(i));  
        
        delay(2000);
    
    }
}

void SetupButtonPinChangeInt()
{   
    // pin change interrupt
    //PCMSK2 = PCINT18 | PCINT19 | PCINT20 | PCINT21;  // want pin D2-D5
    
    // the table at https://playground.arduino.cc/Learning/Pins
    // was helpful to determin PCMSK2.
    PCMSK2 = 0b00111100;  // want pin D2-D5
    
    Serial.print("PCMSK2: ");
    Serial.println(String(PCMSK2));

    delay(2000);

    PCIFR  |= bit (PCIF2);   // clear any outstanding interrupts
    PCICR  |= bit (PCIE2);   // enable pin change interrupts for  D2-D5
    
}


ISR (PCINT2_vect)
{
    //PCINT2_vect -> port d -> pins d2 - d5.
    // toggle LED

    int value = ++pushed % 2;
    digitalWrite(LEDWAKE, value);
}

void setup () 
{
    Serial.begin(115200);

    while(!Serial) {}

    delay(5000);

    pinMode (LEDWAKE, OUTPUT);
    
    digitalWrite(LEDWAKE, HIGH);

    
    SetupButtonPins();
    SetupButtonPinChangeInt();

}  // end of setup

void loop () 
{
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_mode ();  

  // flash to indicate we got out of sleep
//   digitalWrite (LEDLOOP, HIGH);
//   delay (100);
//   digitalWrite (LEDLOOP, LOW);
//   delay (100);
  
  } // end of loop



// /*
// This is an American Motorcycle Association Enduro computer.
// Given a route of speed averages, resets, free time, and known controls
// this computer will give the user their pace, distance, and time,
// all the information needed to win!

//    Hardware setup:
// Microcontroller:
//    Arduino Nano 238P
// RTC / EEPROM:
//    ZS-042 break out board
//    DS3231  RTC
//    AT24C32 32kbit eeprom

// Connections:
//    ZS-042 is connected to the arduino with i2c.
//    IIC sac and sad lines are on pins a4 and a5
//    it is powered with 5v from the teensy
//    External pull up resistors are used.
//    I think they are 5k.

//     LCD
//     The remaining analog pins are used to drive the LCD.

//     Buttons:
//     Four buttons with hardware debouncing are on pins d2 - d5.
//     That is port D
//     74hc14 schmit trigger is used with an RC filter handel the debouncing.

//     Wheel Sensor:
//     d8
//     Port B
// */


// #include <Arduino.h>
// #include <avr/interrupt.h>

// #include "display.h"
// #include "WheelManager.h"
// #include "SpeedInterval.h"

// #include <Wire.h> //https://forum.pjrc.com/threads/21680-New-I2C-library-for-Teensy3
// //#include <i2c_t3.h>
// // #include <hd44780.h>                       // main hd44780 header
// // #include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// #include <LiquidCrystal.h>

// #include "TimeService.h"
// #include "EepromIic.h"
// #include "Route.h"
// #include "EnduroManager.h"

// //hd44780_I2Cexp lcd; // declare lcd object: auto locate & config exapander chip

// TimeServicKnh timeKnh;
// WheelManager wm;
// EepromIic eepromIic;
// EnduroManager em;


// // initialize the library by associating any needed LCD interface pin
// // with the arduino pin number it is connected to
// const int rs = PIN_A0, en = PIN_A1, d4 = PIN_A2, d5 = PIN_A3, d6 = PIN_A6, d7 = PIN_A7;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// // buttons

// const int buttonPinA = 4;
// volatile int buttonPushed = 0; // 0 -> false, 1 -> true.
// // buttons

// void setup() 
// {

// //   // set up the LCD's number of columns and rows:
//   lcd.begin(20, 4);
//   // Print a message to the LCD.
//   lcd.print("hello, world!");

//     Serial.begin(115200);

//     while(!Serial) {}

//     Wire.begin();

//     delay(100);

//     #if WHEEL_MANAGER_DEBUG == 1
//     //wmTest();
//     #endif

//     #if ROUTE_DEBUG == 1
//     RouteTest();
//     #endif
    
//     #if ENDURO_MANAGER_DEBUG == 1
//     EnduroManagerTest();
//     #endif

//     // use pin change interupt on port b for wheel sensor
//     // use pin change interupt on port d for buttons
//     PCICR |= 0b00000001;    // turn on port b
//     //PCICR |= 0b00000010;    // turn on port c
//     PCICR |= 0b00000100;    // turn on port d

//     int a = PCIE0;

//     PCMSK2 |= PCINT20;


//     PCIFR  |= bit (PCIF2);   // clear any outstanding interrupts

//     ///ISR(PCINT0_vect){}    // Port B, PCINT0 - PCINT7
//     //ISR(PCINT1_vect){}    // Port C, PCINT8 - PCINT14
//     //ISR(PCINT2_vect){}    // Port D, PCINT16 - PCINT23

//     // pinMode(LED_BUILTIN, OUTPUT);
//     // pinMode(19, INPUT_PULLUP); 
//     // pinMode(20, INPUT_PULLUP); 
//     // pinMode(21, INPUT_PULLUP); 
//     // pinMode(22, INPUT_PULLUP); 

//     pinMode(4, INPUT);
//     digitalWrite(4, HIGH);
// }

// ISR(PCINT0_vect)
// {
//     buttonPushed = true;
// }

// float tenthMilesToPossiable = 0;
// int16_t secondsOffPace = 0;

// uint32_t i = 0;

// bool ledOn = false;
// void loop() {

//     if(buttonPushed > 0)
//     {
//         Serial.print(F("button pushed: "));
//         buttonPushed = 0;
//     }

//     // read works
//     int p = digitalRead(4);


//     Serial.print(F("p: "));
//     Serial.println(p);

//     // uint8_t read = digitalRead(PIN_A7);
//     // Serial.printf("read %d\n", read);
    
//     // if(read) ledOn = !ledOn;

//     // if(ledOn)
//     //     digitalWrite(LED_BUILTIN, HIGH);
//     // else
//     //     digitalWrite(LED_BUILTIN, LOW);

//     // delay(1000);


//     // if(i++ == 0) em.startEnduro();

//     // uint32_t microSecondsStart = millis();

//     // wm.AddTickRaw();

//     // em.getRaceData( 
//     //     tenthMilesToPossiable,
//     //     secondsOffPace);

//     // float distance = wm.GetTotalDistance();

//     // uint32_t durationMicroSeconmds = millis() - microSecondsStart;
    
//     // Serial.print(F("durationMicroSeconmds: "));
//     // Serial.println(durationMicroSeconmds);

//     // Serial.print(F("getRaceSeconds: "));
//     // Serial.println(timeKnh.getRaceSeconds());

//     // Serial.print(F("distance: "));
//     // Serial.println(distance);

//     // Serial.print(F("speed: "));
//     // Serial.println(wm.GetSpeed(2));

//     // Serial.print(F("tenthMilesToPossiable: "));
//     // Serial.println(tenthMilesToPossiable);

//     // Serial.print(F("secondsOffPace: "));
//     // Serial.println(secondsOffPace);


//     // Serial.println("");

// }