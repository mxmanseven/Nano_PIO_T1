
#include <Arduino.h>
#include <avr/interrupt.h>
#include "Buttons.h"


ButtonInfo::ButtonInfo()
{
    lastReadTime = 0;
    down = false;
    pressedShort = false;
    pressedLong = false;
    pin = -1;
}

ButtonInfo Buttons::pins[PIN_COUNT];

Buttons::Buttons()
{
}

int Buttons::Init(
    uint8_t leftPin, 
    uint8_t rightPin, 
    uint8_t upPin, 
    uint8_t downPin)
{
    pins[0].pin = leftPin;
    pins[1].pin = rightPin;
    pins[2].pin = upPin;
    pins[3].pin = downPin;

    // set pin mode to input pull up

    // set pin change interupt and mask
}


void Buttons::SetupPins()
{
    for(int i =  0; i < PIN_COUNT; i++)
    {
        uint8_t pin = pins[i].pin;   
        pinMode(pin, INPUT);
        digitalWrite (pin, HIGH);  // enable pull-up  
        Serial.print("Set Pin as high input: ");
        Serial.println(String(i));  
        
        delay(2000);
    
    }

    // pin change interrupt
  
  // knh todo - update mask and port by selected pins

    // the table at https://playground.arduino.cc/Learning/Pins
    // was helpful to determin PCMSK2.
    PCMSK2 = 0b00111100;  // want pin D2-D5
    
    Serial.print("PCMSK2: ");
    Serial.println(String(PCMSK2));

    delay(2000);

    PCIFR  |= bit (PCIF2);   // clear any outstanding interrupts
    PCICR  |= bit (PCIE2);   // enable pin change interrupts for  D2-D5
    
}

