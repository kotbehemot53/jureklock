//
// Created by behemot on 28.01.2024.
//

#ifndef JUREKLOCK1_STATUSLEDOUTPUT_H
#define JUREKLOCK1_STATUSLEDOUTPUT_H

#include <Arduino.h>

#define _TASK_OO_CALLBACKS      // Support for dynamic callback method binding
#include <TaskSchedulerDeclarations.h>

// TODO: introduce separate classes for each task (single blink, double blink, etc)?
//       or maybe just inherit Task here and add those methods which set the correct params and run this->reset?

class StatusLEDOutput : public Task
{
private:
    uint8_t ledPin;

    StatusLEDOutput();

public :
    StatusLEDOutput(uint8_t ledPin, Scheduler* ts);
    ~StatusLEDOutput() {}; // TODO?

//    static void setup(Scheduler* ts, uint8_t ledPin);

    void statusLEDOn();
    void statusLEDOff();

    void shortBlink();
    void longBlink();
    void doubleBlink();
    void tripleBlink();

    bool Callback();
//
//    static Task* LEDBlinkingTask;//(0, 0, &blinkCallback, &ts, false);
};


#endif //JUREKLOCK1_STATUSLEDOUTPUT_H
