//
// Created by behemot on 30.01.2024.
//

#include "StateResetScheduler.h"

StateResetScheduler::StateResetScheduler()
{
}

StateResetScheduler::StateResetScheduler(Scheduler* ts, StateManager *aStateManager, ScreenOutput *aScreenOutput) : Task(0, 1, ts, false)
{
    this->stateManager = aStateManager;
    this->screenOutput = aScreenOutput;
}

void StateResetScheduler::scheduleReset(int timeout)
{
    this->restartDelayed(timeout);
}

bool StateResetScheduler::Callback()
{
    this->stateManager->reset();
    this->screenOutput->clearScreen();
}
