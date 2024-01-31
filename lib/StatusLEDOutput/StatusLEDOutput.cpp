//
// Created by behemot on 28.01.2024.
//

#include "StatusLEDOutput.h"

StatusLEDOutput::StatusLEDOutput()
{
}

StatusLEDOutput::StatusLEDOutput(uint8_t aLedPin, Scheduler* ts): Task(0,0,ts,false)
{
    ledPin = aLedPin;
}

void StatusLEDOutput::init()
{
    pinMode(this->ledPin, OUTPUT);
    digitalWrite(this->ledPin, LOW);
}

void StatusLEDOutput::statusLEDOn() const
{
    digitalWrite(this->ledPin, HIGH);
}

void StatusLEDOutput::statusLEDOff() const
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

    return false;
}
