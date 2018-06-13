
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

String ButtonInfo::ToString()
{
    return
        "pin " +  String(pin)
        + " pressedShort " +  String(pressedShort)
        + " pressedLong " +  String(pressedLong)
        + " Down " +  String(down);
}

Buttons::Buttons()
{
}

int Buttons::Init(
    uint8_t leftPin, 
    uint8_t rightPin, 
    uint8_t upPin, 
    uint8_t downPin)
{
    int8_t nResult = 0;

    pins[0].pin = leftPin;
    pins[1].pin = rightPin;
    pins[2].pin = upPin;
    pins[3].pin = downPin;

    return nResult;
}

void Buttons::SetupPins()
{
    // set pin mode to input and enable pull-up
    for(int i =  0; i < PIN_COUNT; i++)
    {
        uint8_t pin = pins[i].pin;   
        pinMode(pin, INPUT);
        digitalWrite (pin, HIGH);  // enable pull-up  
        Serial.print("Set Pin as high input: ");
        Serial.println(String(i));
        delay(200);    
    }

    // pin change interrupt
  
  // knh todo - update mask and port by selected pins

    // the table at https://playground.arduino.cc/Learning/Pins
    // was helpful to determin PCMSK2.
    PCMSK2 = 0b00111100;  // want pin D2-D5    
    Serial.print("PCMSK2: ");
    Serial.println(String(PCMSK2));

    delay(200);

    PCIFR  |= bit (PCIF2);   // clear any outstanding interrupts
    PCICR  |= bit (PCIE2);   // enable pin change interrupts for  D2-D5    
}

// the consumer of Buttons should not change down field because we depend on it here.
int Buttons::ReadButtons()
{
    // This should be called after a pin change interupt
    // That means that some thing about the buttons state is new.

    int8_t nResult = 0;

    uint32_t nowMs = millis();

    for(int i =  0; i < PIN_COUNT; i++)
    {
        ButtonInfo* buttonInfo = &pins[i];
        // buttonInfo->pressedLong = false;
        // buttonInfo->pressedShort = false;

        int buttonStateNew = digitalRead(buttonInfo->pin);

        if(buttonInfo->down == false
            && buttonStateNew == 1)
        {
            // the button was not down before but is now
            buttonInfo->down = true;
            buttonInfo->lastReadTime = nowMs;
        }
        else if (buttonInfo->down == true
            && buttonStateNew == 0)
        {
            // was down but is no longer
            buttonInfo->down = false;

            uint32_t duration = nowMs - buttonInfo->lastReadTime;

            if(duration > ButtonInfo::LONG_PRESS_MIN_THRESHOLD_MILLI_SECONDS)
            {
                buttonInfo->pressedLong = true;
                buttonInfo->pressedShort = false;
            }
            else 
            {
                buttonInfo->pressedLong = false;
                buttonInfo->pressedShort = true;
            }
        }

        Serial.println(buttonInfo->ToString());
    }
    
    Serial.println();

    return nResult;
}