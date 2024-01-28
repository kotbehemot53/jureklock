//
// Created by behemot on 21.01.2024.
//

#ifndef JUREKLOCK1_LOCK_H
#define JUREKLOCK1_LOCK_H

#include <Arduino.h>

#define _TASK_OO_CALLBACKS      // Support for dynamic callback method binding
#include <TaskSchedulerDeclarations.h>

class Lock : public Task
{
private:
    Lock();

    byte doorPin;
    int doorOpenTime;

public:
    Lock(byte doorPin, Scheduler* ts, int aDoorOpenTime);

    inline void unlock() { digitalWrite(this->doorPin, HIGH); }
    inline void lock() { digitalWrite(this->doorPin, LOW); }
    inline bool isUnlocked() { return digitalRead(this->doorPin); }

    void unlockDoorAndScheduleLocking();
    bool Callback();
};

#endif //JUREKLOCK1_LOCK_H
