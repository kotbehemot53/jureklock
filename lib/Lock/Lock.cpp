//
// Created by behemot on 27.12.2023.
//
#include "Lock.h"

void Lock::setCode(unsigned char newCode[4])
{
    for (short i = 0; i < 4; i++) {
        this->code[i] = newCode[i];
    }
}

unsigned char* Lock::getCode()
{
    return this->code;
}

bool Lock::checkCode(unsigned char checkedCode[4])
{
    for (short i = 0; i < 4; i++) {
        if (this->code[i] != checkedCode[i]) {
            return false;
        }
    }

    return true;
}

