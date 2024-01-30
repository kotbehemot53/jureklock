//
// Created by behemot on 21.01.2024.
//

#include "CodeBuffer.h"

void CodeBuffer::addDigit(unsigned char receivedChar)
{
    this->buffer[this->bufferPtr++] = receivedChar;
}

unsigned char* CodeBuffer::getCode()
{
    return buffer;
}

short CodeBuffer::getNumberOfDigitsReceived()
{
    return this->bufferPtr;
}

void CodeBuffer::reset()
{
    this->bufferPtr = 0;
}

bool CodeBuffer::isCompleteCodeReceived()
{
    if (bufferPtr >= this->bufferLength) {
        return true;
    }

    return false;
}
