//
// Created by behemot on 30.01.2024.
//

#ifndef JUREKLOCK1_STATERESETSCHEDULER_H
#define JUREKLOCK1_STATERESETSCHEDULER_H

#include "ScreenOutput.h"
#include "StateManager.h"

class StateResetScheduler : private Task
{
private:
    StateManager* stateManager;
    ScreenOutput* screenOutput;

    StateResetScheduler();

    bool Callback();

public:
    StateResetScheduler(Scheduler* ts, StateManager *aStateManager, ScreenOutput *aScreenOutput);

    void scheduleReset(int timeout);
};


#endif //JUREKLOCK1_STATERESETSCHEDULER_H
