//
// Created by behemot on 21.01.2024.
//

#ifndef JUREKLOCK1_CODEBUFFER_H
#define JUREKLOCK1_CODEBUFFER_H

#include <Arduino.h>

//#include "StateManager.h"

class CodeBuffer
{
private:
    static const byte bufferLength = 4;

    unsigned char buffer[bufferLength];
    short bufferPtr = 0;

//protected:

public:
    void addDigit(unsigned char receivedChar);

    void reset();
    bool isCompleteCodeReceived();
    short getNumberOfDigitsReceived();
    unsigned char* getCode();

//    friend class StateManager;
};

#endif //JUREKLOCK1_CODEBUFFER_H
