//
// Created by behemot on 30.01.2024.
//

#ifndef JUREKLOCK1_STATERESETSCHEDULER_H
#define JUREKLOCK1_STATERESETSCHEDULER_H

#include "ScreenOutput.h"
#include "StateManager.h"

class StateResetScheduler : public Task
{
private:
    StateManager* stateManager;
    ScreenOutput* screenOutput;

public:
    StateResetScheduler(Scheduler* ts, StateManager *aStateManager, ScreenOutput *aScreenOutput);

    void scheduleReset(int timeout);

    bool Callback();
};


#endif //JUREKLOCK1_STATERESETSCHEDULER_H
