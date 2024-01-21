//
// Created by behemot on 21.01.2024.
//

#include "CodeBuffer.h"

void CodeBuffer::listenForCodeToOpen()
{
    this->bufferPtr = 0;
    this->listeningForCodeToOpen = true;
    this->listeningForCodeToChange = false;
}

void CodeBuffer::listenForCodeToChange()
{
    this->bufferPtr = 0;
    this->listeningForCodeToOpen = false;
    this->listeningForCodeToChange = true;
}

void CodeBuffer::stopListeningForCode()
{
    this->bufferPtr = -1;
    this->listeningForCodeToOpen = false;
    this->listeningForCodeToChange = false;
}

bool CodeBuffer::isListeningForCodeToOpen()
{
    return listeningForCodeToOpen;
}

bool CodeBuffer::isListeningForCodeToChange()
{
    return listeningForCodeToChange;
}

void CodeBuffer::addDigit(unsigned char receivedChar)
{
    if (this->bufferPtr < 0) {
        // TODO: exception?
    } else {

        this->buffer[this->bufferPtr++] = receivedChar;
        if (bufferPtr >= this->bufferLength) {
            this->stopListeningForCode();
        }
    }
}

unsigned char* CodeBuffer::getCode()
{
    return buffer;
}

short CodeBuffer::getNumberOfDigitsReceived()
{
    return this->bufferPtr;
}
