
#include <Arduino.h>
#include <avr/interrupt.h>
#include "WheelListener.h"

WheelListener::WheelListener() {}

int WheelListener::Init()
{
    
    // the table at https://playground.arduino.cc/Learning/Pins
    // was helpful to determin PCMSK2.
    PCMSK0 = 0b00000001;  // want pin D8 / PB0 / 14    
    Serial.print("PCMSK0: ");
    Serial.println(String(PCMSK0));

    delay(200);

    PCIFR  |= bit (PCIF0);   // clear any outstanding interrupts
    PCICR  |= bit (PCIE0);   // enable pin change interrupts for  D2-D5    
}