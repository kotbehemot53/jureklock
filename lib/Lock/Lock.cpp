//
// Created by behemot on 21.01.2024.
//

#include "Lock.h"

Lock::Lock() {}

Lock::Lock(byte aDoorPin)
{
    doorPin = aDoorPin;
}

void Lock::init()
{
    pinMode(this->doorPin, OUTPUT);
    digitalWrite(this->doorPin, LOW);
}
