#ifndef BUTTONS_H
#define BUTTONS_H


#include <avr/io.h>

class ButtonInfo
{   
    public:
    ButtonInfo();
    uint32_t lastReadTime;
    bool down;
    bool pressedShort;
    bool pressedLong;
    int8_t pin;
    
    static const int LONG_PRESS_MIN_THRESHOLD_MILLI_SECONDS = 2000;

    String ToString();
}; 

class Buttons
{
    public:
    Buttons();
    int Init(
        uint8_t leftPin, 
        uint8_t rightPin, 
        uint8_t upPin, 
        uint8_t downPin);

    void SetupPins();

    static const uint8_t PIN_COUNT = 4;
    static ButtonInfo pins[PIN_COUNT];

    int ReadButtons();
};

#endif