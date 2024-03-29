//
// Created by behemot on 28.01.2024.
//

#include "LockScheduler.h"

LockScheduler::LockScheduler()
{
}

LockScheduler::LockScheduler(Scheduler* ts, Lock* aLock, StatusLEDOutput* aStatusLedOutput, int aDoorOpenTime):
    Task(0, 1, ts, false)
{
    this->lock = aLock;
    this->statusLedOutput = aStatusLedOutput;
    this->doorOpenTime = aDoorOpenTime;
}

bool LockScheduler::Callback()
{
    this->lock->lock();
    this->statusLedOutput->statusLEDOff();

    return false;
}

void LockScheduler::unlockDoorAndScheduleLocking()
{
    this->lock->unlock();
    this->statusLedOutput->statusLEDOn();
    this->restartDelayed(this->doorOpenTime);
}