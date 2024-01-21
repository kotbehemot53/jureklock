//
// Created by behemot on 21.01.2024.
//

#ifndef JUREKLOCK1_LOCK_H
#define JUREKLOCK1_LOCK_H

#include <Arduino.h>

class Lock
{
private:
    Lock();

    byte doorPin;

public:
    Lock(byte doorPin);

    inline void unlock() { digitalWrite(this->doorPin, HIGH); }
    inline void lock() { digitalWrite(this->doorPin, LOW); }
    inline bool isUnlocked() { return digitalRead(this->doorPin); }
};

#endif //JUREKLOCK1_LOCK_H
