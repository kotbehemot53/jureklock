//
// Created by behemot on 28.01.2024.
//

#ifndef JUREKLOCK1_STATUSLEDOUTPUT_H
#define JUREKLOCK1_STATUSLEDOUTPUT_H

#include <Arduino.h>

#define _TASK_OO_CALLBACKS      // Support for dynamic callback method binding
#include <TaskSchedulerDeclarations.h>

class StatusLEDOutput : private Task
{
private:
    uint8_t ledPin;

    StatusLEDOutput();

    bool Callback();

public :
    StatusLEDOutput(uint8_t ledPin, Scheduler* ts);

    void statusLEDOn() const;
    void statusLEDOff() const;

    void shortBlink();
    void longBlink();
    void doubleBlink();
    void tripleBlink();
};

#endif //JUREKLOCK1_STATUSLEDOUTPUT_H
