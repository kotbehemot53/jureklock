//
// Created by behemot on 30.01.2024.
//

#ifndef JUREKLOCK1_STATEMANAGER_H
#define JUREKLOCK1_STATEMANAGER_H

#include "CodeBuffer.h"

class StateManager
{
private:
    CodeBuffer* codeBuffer;
    bool listeningForCodeToOpen = false;
    bool listeningForCodeToChange = false;
    bool gameInProgress = false;

    StateManager();

    void stopListeningForCode();

public:
    explicit StateManager(CodeBuffer* aCodeBuffer);

    void listenForCodeToOpen();
    void listenForCodeToChange();
    void startGame();
    void stopGame();
    bool isListeningForCodeToOpen();
    bool isListeningForCodeToChange();
    bool isGameInProgress();
    void addDigit(unsigned char receivedChar);
    void reset();
};


#endif //JUREKLOCK1_STATEMANAGER_H
