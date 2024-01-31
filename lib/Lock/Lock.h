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
    explicit Lock(byte doorPin);

    void init();
    inline void unlock() const { digitalWrite(this->doorPin, HIGH); }
    inline void lock() const { digitalWrite(this->doorPin, LOW); }
    inline bool isUnlocked() const { return digitalRead(this->doorPin); }

};

#endif //JUREKLOCK1_LOCK_H
