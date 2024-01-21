//
// Created by behemot on 21.01.2024.
//

#ifndef JUREKLOCK1_CODEBUFFER_H
#define JUREKLOCK1_CODEBUFFER_H

#include <Arduino.h>

class CodeBuffer
{
private:
    static const byte bufferLength = 4;

    unsigned char buffer[bufferLength];
    short bufferPtr = -1;
    bool listeningForCodeToOpen = false;
    bool listeningForCodeToChange = false;

public:
    void listenForCodeToOpen();
    void listenForCodeToChange();
    void stopListeningForCode();
    bool isListeningForCodeToOpen();
    bool isListeningForCodeToChange();
    void addDigit(unsigned char receivedChar);
    short getNumberOfDigitsReceived();
    unsigned char* getCode();
};

#endif //JUREKLOCK1_CODEBUFFER_H
