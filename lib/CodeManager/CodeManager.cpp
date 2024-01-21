//
// Created by behemot on 27.12.2023.
//
#include "CodeManager.h"
#include <string.h>
#include <EEPROM.h>

CodeManager::CodeManager() {
}

CodeManager::CodeManager(const unsigned char defaultCode[4]): defaultCode(defaultCode)
{
    memcpy(this->code, this->defaultCode, 4*sizeof(unsigned char));

    loadCode();
}

void CodeManager::setCode(unsigned char newCode[4])
{
    for (short i = 0; i < 4; i++) {
        this->code[i] = newCode[i];
    }
}

unsigned char* CodeManager::getCode()
{
    return this->code;
}

bool CodeManager::checkCode(unsigned char checkedCode[4])
{
    for (short i = 0; i < 4; i++) {
        if (this->code[i] != checkedCode[i]) {
            return false;
        }
    }

    return true;
}

void CodeManager::saveCode(unsigned char* codeBuffer)
{
    for (short i = 0; i < 4; i++) {
        EEPROM.write(i, codeBuffer[i]);
    }
    this->setCode(codeBuffer);
}

void CodeManager::loadCode()
{
    for (short i = 0; i < 4; i++) {
        this->code[i] = EEPROM.read(i);
    }
}

void CodeManager::factoryReset()
{
    memcpy(this->code, this->defaultCode, 4*sizeof(unsigned char));
}