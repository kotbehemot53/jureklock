//
// Created by behemot on 28.01.2024.
//

#include "StatusLEDOutput.h"
//#include <functional>
//
StatusLEDOutput::StatusLEDOutput()
{
}
//
StatusLEDOutput::StatusLEDOutput(uint8_t aLedPin, Scheduler* ts): Task(0,0,ts,false)
{
    ledPin = aLedPin;
}

//Task StatusLEDOutput::LEDBlinkingTask(0, 0, &blinkCallback, &ts, false);

//void StatusLEDOutput::setup(Scheduler* ts, uint8_t aLedPin)
//{
//    StatusLEDOutput::ledPin = aLedPin;
//    // TODO: no destructor needed right?
//    StatusLEDOutput::LEDBlinkingTask = new Task(0, 0, &StatusLEDOutput::blinkCallback, ts, false);
//}

void StatusLEDOutput::statusLEDOn()
{
    digitalWrite(this->ledPin, HIGH);
}

void StatusLEDOutput::statusLEDOff()
{
    digitalWrite(this->ledPin, LOW);
}

void StatusLEDOutput::shortBlink()
{
    this->disable();
    this->setInterval(50);
    this->setIterations(2);
    this->enable();
}

void StatusLEDOutput::longBlink()
{
    this->disable();
    this->setInterval(1000);
    this->setIterations(2);
    this->enable();
}

void StatusLEDOutput::doubleBlink()
{
    this->disable();
    this->setInterval(100);
    this->setIterations(4);
    this->enable();
}

void StatusLEDOutput::tripleBlink()
{
    this->disable();
    this->setInterval(100);
    this->setIterations(6);
    this->enable();
}

bool StatusLEDOutput::Callback()
{
    if (this->getRunCounter() & 1) {
        this->statusLEDOn();
    } else {
        this->statusLEDOff();
    }

    if (this->isLastIteration()) {
        this->statusLEDOff();
    }
}
