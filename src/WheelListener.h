#ifndef WHEEL_LISTENER_H
#define WHEEL_LISTENER_H

#include <avr/io.h>

class WheelListener
{
    public:
    WheelListener();
    int Init();

    void SetupPins();
};

#endif