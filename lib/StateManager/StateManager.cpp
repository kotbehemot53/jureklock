//
// Created by behemot on 30.01.2024.
//

#include "StateManager.h"

StateManager::StateManager()
{
}

StateManager::StateManager(CodeBuffer *aCodeBuffer) : codeBuffer(aCodeBuffer)
{
}

void StateManager::listenForCodeToOpen()
{
    this->stopGame();
    this->codeBuffer->reset();
    this->listeningForCodeToOpen = true;
    this->listeningForCodeToChange = false;
}

void StateManager::listenForCodeToChange()
{
    this->stopGame();
    this->codeBuffer->reset();
    this->listeningForCodeToOpen = false;
    this->listeningForCodeToChange = true;
}

void StateManager::startGame()
{
    this->stopListeningForCode();
    this->gameInProgress = true;
}

void StateManager::stopGame()
{
    this->gameInProgress = false;
}

bool StateManager::isListeningForCodeToOpen()
{
    return listeningForCodeToOpen;
}

bool StateManager::isListeningForCodeToChange()
{
    return listeningForCodeToChange;
}

bool StateManager::isGameInProgress()
{
    return gameInProgress;
}

void StateManager::addDigit(unsigned char receivedChar)
{
    this->codeBuffer->addDigit(receivedChar);
    if (this->codeBuffer->isCompleteCodeReceived()) {
        this->stopListeningForCode();
    }
}

void StateManager::reset()
{
    this->stopListeningForCode();
    this->stopGame();
}


void StateManager::stopListeningForCode()
{
    this->codeBuffer->reset();
    this->listeningForCodeToOpen = false;
    this->listeningForCodeToChange = false;
}
