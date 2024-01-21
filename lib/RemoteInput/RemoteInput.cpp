//
// Created by behemot on 21.01.2024.
//

#include "RemoteInput.h"

RemoteInput::RemoteInput(
    uint8_t commandEnterCodeToOpen,
    uint8_t commandEnterCodeToChange,
    uint8_t commandStartGame,
    uint8_t commandJump,
    uint8_t* commandsNumeric
):
    commandsNumeric(commandsNumeric),
    commandEnterCodeToOpen(commandEnterCodeToOpen),
    commandEnterCodeToChange(commandEnterCodeToChange),
    commandStartGame(commandStartGame),
    commandJump(commandJump)
{}

bool RemoteInput::hasNewCommand(volatile RemoteInputReceivedData* receivedData)
{
    // TODO: disable interrupts for the duration of this function? (reading from receivedData)
    if (!receivedData->justWritten) {
        return false;
    }

    receivedData->justWritten = false;

    this->setReceivedCommand(receivedData->Command);

    return !receivedData->isRepeat && !receivedData->parityFailed;
}

uint8_t RemoteInput::getCommand()
{
    return this->receivedCommand;
}

void RemoteInput::setReceivedCommand(uint8_t command)
{
    this->receivedCommand = command;
}

bool RemoteInput::isCommandNumeric(uint8_t command)
{
    for (byte i = 0; i < 10; ++i) {
        if (this->commandsNumeric[i] == command) {
            return true;
        }
    }

    return false;
}
