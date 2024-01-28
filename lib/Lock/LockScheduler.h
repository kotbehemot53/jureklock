//
// Created by behemot on 28.01.2024.
//

#ifndef JUREKLOCK1_LOCKSCHEDULER_H
#define JUREKLOCK1_LOCKSCHEDULER_H

#define _TASK_OO_CALLBACKS      // Support for dynamic callback method binding
#include <TaskSchedulerDeclarations.h>

#include "Lock.h"
#include "StatusLEDOutput.h"

class LockScheduler : public Task
{
private:
    int doorOpenTime;
    Lock* lock;
    StatusLEDOutput* statusLedOutput;

public:
    LockScheduler(Scheduler* ts, Lock* aLock, StatusLEDOutput* aStatusLedOutput, int aDoorOpenTime);

    void unlockDoorAndScheduleLocking();
    bool Callback();
};


#endif //JUREKLOCK1_LOCKSCHEDULER_H
