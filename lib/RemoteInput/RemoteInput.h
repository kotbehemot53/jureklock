//
// Created by behemot on 21.01.2024.
//

#ifndef JUREKLOCK1_REMOTEINPUT_H
#define JUREKLOCK1_REMOTEINPUT_H

#include <Arduino.h>

struct RemoteInputReceivedData {
    uint8_t Command;
    bool isRepeat;
    bool parityFailed;
    bool justWritten;
};

class RemoteInput
{
private:
    const uint8_t* commandsNumeric;

    uint8_t receivedCommand;

    void setReceivedCommand(uint8_t command);

public:
    const uint8_t commandEnterCodeToOpen;
    const uint8_t commandEnterCodeToChange;
    const uint8_t commandStartGame;
    const uint8_t commandJump;

    RemoteInput(
        uint8_t commandEnterCodeToOpen,
        uint8_t commandEnterCodeToChange,
        uint8_t commandStartGame,
        uint8_t commandJump,
        uint8_t* commandsNumeric
    );

    bool hasNewCommand(volatile RemoteInputReceivedData* receivedData);
    uint8_t getCommand();
    bool isCommandNumeric(uint8_t command);
};


#endif //JUREKLOCK1_REMOTEINPUT_H
