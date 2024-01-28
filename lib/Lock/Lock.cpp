//
// Created by behemot on 21.01.2024.
//

#include "Lock.h"

Lock::Lock() {}

Lock::Lock(byte aDoorPin, Scheduler* ts, int aDoorOpenTime): Task(0, 1, ts, false)
{
    doorPin = aDoorPin;
    doorOpenTime = aDoorOpenTime;
}

bool Lock::Callback()
{
    this->lock();
    // TODO: LED + Screen say? here?
}

void Lock::unlockDoorAndScheduleLocking()
{
    this->unlock();
    // TODO: LED? Screen say? here?
    this->restartDelayed(this->doorOpenTime);
}
