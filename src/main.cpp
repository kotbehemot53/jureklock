#include <Arduino.h>
#include <avr/wdt.h>

#define IR_RECEIVE_PIN 2

#include <TinyIRReceiver.hpp>
#include <OneButtonTiny.h>
#include <U8g2lib.h>

#define _TASK_OO_CALLBACKS
#include <TaskScheduler.h>

#include "CodeManager.h"
#include "Lock.h"
#include "LockScheduler.h"
#include "Game.h"
#include "RemoteInput.h"
#include "CodeBuffer.h"
#include "StatusLEDOutput.h"
#include "ScreenOutput.h"

#define DOOR_PIN 8
#define NEG_RESET_PIN 9
#define STATUS_LED_PIN 6


#ifdef WOKWI_ENABLED

#define BTN_UP 2
#define BTN_DN 152
#define BTN_L 224
#define BTN_R 144
#define BTN_OK 168
#define BTN_ASTR 34
#define BTN_HASH 194
#define BTN_0 104
#define BTN_1 48
#define BTN_2 24
#define BTN_3 122
#define BTN_4 16
#define BTN_5 56
#define BTN_6 90
#define BTN_7 66
#define BTN_8 74
#define BTN_9 82

#else

#define BTN_UP 0x18
#define BTN_DN 0x52
#define BTN_L 0x8
#define BTN_R 0x5A
#define BTN_OK 0x1C
#define BTN_ASTR 0x16
#define BTN_HASH 0xD
#define BTN_0 0x19
#define BTN_1 0x45
#define BTN_2 0x46
#define BTN_3 0x47
#define BTN_4 0x44
#define BTN_5 0x40
#define BTN_6 0x43
#define BTN_7 0x7
#define BTN_8 0x15
#define BTN_9 0x9

#endif

#define DOOR_OPEN_TIME_MS 10000 // 10 s

// TODO:
//      1. refactor (OOP)
//      2. migrate to avr DA series?
//      3. 5V buck converter?
//      4. game freezing randomly on real hardware - why? and why didn't it freeze at one revision (see tags)?

//inline const char* findButtonName(unsigned char code)
//{
//    switch (code) {
//        case BTN_HASH:
//            return "#";
//        case BTN_ASTR:
//            return "*";
//        case BTN_0:
//            return "0";
//        case BTN_1:
//            return "1";
//        case BTN_2:
//            return "2";
//        case BTN_3:
//            return "3";
//        case BTN_4:
//            return "4";
//        case BTN_5:
//            return "5";
//        case BTN_6:
//            return "6";
//        case BTN_7:
//            return "7";
//        case BTN_8:
//            return "8";
//        case BTN_9:
//            return "9";
//        case BTN_DN:
//            return "DOWN";
//        case BTN_UP:
//            return "UP";
//        case BTN_L:
//            return "LEFT";
//        case BTN_R:
//            return "RIGHT";
//        case BTN_OK:
//            return "OK";
//    }
//    return "";
//}

volatile struct RemoteInputReceivedData remoteInterruptData;
static void factoryReset();
Scheduler ts;

unsigned const char defaultCode[4] = {BTN_1,BTN_1,BTN_1,BTN_1};
CodeManager codeManager(defaultCode);
Lock lock(DOOR_PIN);
uint8_t numberButtons[10] = {BTN_0, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7, BTN_8, BTN_9};
RemoteInput remoteInput(
    BTN_ASTR,
    BTN_HASH,
    BTN_OK,
    BTN_UP,
    numberButtons
);
CodeBuffer codeBuffer;
StatusLEDOutput statusLedOutput(STATUS_LED_PIN, &ts);

// TODO: what about these:
OneButtonTiny* resetBtn;
uint8_t receivedCommand;

// game
Game game;

// TODO: game orchestration? and drawing?
bool gameInProgress = false;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
// TODO: construct u8g2 inside screenOutput? or game drawing class?
//       or not, because both screen and game drawing should reuse the same object?
//       or should game drawing use screen output???
ScreenOutput screenOutput(&ts, &game, &u8g2);

// TODO: debug class?
//void printCurrentCode();

LockScheduler lockScheduler(&ts, &lock, &statusLedOutput, DOOR_OPEN_TIME_MS);


void setup() {
    pinMode(DOOR_PIN, OUTPUT);
    pinMode(STATUS_LED_PIN, OUTPUT);

    digitalWrite(DOOR_PIN, LOW);
    digitalWrite(STATUS_LED_PIN, LOW);

    resetBtn = new OneButtonTiny(NEG_RESET_PIN);
    resetBtn->attachClick(factoryReset);

    initPCIInterruptForTinyReceiver();

//    Serial.begin(115200);
//    Serial.println("Hullo");

    // save a default code if non has been set yet
    if (
        !remoteInput.isCommandNumeric(codeManager.getCode()[0]) ||
        !remoteInput.isCommandNumeric(codeManager.getCode()[1]) ||
        !remoteInput.isCommandNumeric(codeManager.getCode()[2]) ||
        !remoteInput.isCommandNumeric(codeManager.getCode()[3])
    ) {
        codeManager.factoryReset();
    }

    u8g2.begin();

    randomSeed(analogRead(0));

    wdt_enable(WDTO_2S);
}

void handleReceivedTinyIRData(uint8_t aAddress, uint8_t aCommand, uint8_t aFlags)
{
    remoteInterruptData.Command = aCommand;
    remoteInterruptData.isRepeat = aFlags == IRDATA_FLAGS_IS_REPEAT;
    remoteInterruptData.parityFailed = aFlags == IRDATA_FLAGS_PARITY_FAILED;
    remoteInterruptData.justWritten = true;
}

void loop() {
    wdt_reset();

    ts.execute();
    resetBtn->tick();

    // TODO: move it all to some game orchestrator?
    // draw game
    if (gameInProgress) {
        if (game.isGameOver()) {
            screenOutput.drawGameOver();
            gameInProgress = false;
        } else {
            screenOutput.drawGame();
        }

        game.tick();
    }

    // handle input
    if (remoteInput.hasNewCommand(&remoteInterruptData)) {
        receivedCommand = remoteInput.getCommand();

        // start listening for code
        if (receivedCommand == remoteInput.commandEnterCodeToOpen) {
            gameInProgress = false;

            codeBuffer.listenForCodeToOpen();
            statusLedOutput.shortBlink();

            screenOutput.say(F("Dawaj kod!"));
        } else if (codeBuffer.isListeningForCodeToOpen() && remoteInput.isCommandNumeric(receivedCommand)) {
            screenOutput.drawStar(codeBuffer.getNumberOfDigitsReceived());

            codeBuffer.addDigit(receivedCommand);

            // full code received
            if (!codeBuffer.isListeningForCodeToOpen()) {
                if (codeManager.checkCode(codeBuffer.getCode())) {
                    lockScheduler.unlockDoorAndScheduleLocking();

                    screenOutput.say(F("Otwarte!"));
                } else {
                    statusLedOutput.doubleBlink();
                    screenOutput.say(F("Lipa, panie!"));
                }
            }
        } else if (receivedCommand == remoteInput.commandEnterCodeToChange && lock.isUnlocked()) {
            codeBuffer.listenForCodeToChange();
            statusLedOutput.tripleBlink();

            screenOutput.say(F("Nowy kod?"));
        } else if (codeBuffer.isListeningForCodeToChange() && remoteInput.isCommandNumeric(receivedCommand)) {
            screenOutput.drawStar(codeBuffer.getNumberOfDigitsReceived());

            codeBuffer.addDigit(receivedCommand);

            // full code received
            if (!codeBuffer.isListeningForCodeToChange()) {
                codeManager.saveCode(codeBuffer.getCode());

                screenOutput.say(F("Ustawiony!"));
//                    printCurrentCode();
                statusLedOutput.longBlink();
            }
        } else if (receivedCommand == remoteInput.commandStartGame) {
            if (gameInProgress) {
                // TODO: move gameInProgress to game?
                gameInProgress = false;
                screenOutput.scheduleClear(1);
            } else {
                codeBuffer.stopListeningForCode();

                game.initGame();
                gameInProgress = true;

                // TODO:  separate const for that?
                // TODO: set gameInProgress to false on clear!
                // TODO: actually keep all these waits/progresses in some orchestrator and resetll ALL on all clears? or not?
                screenOutput.scheduleClear(DOOR_OPEN_TIME_MS);
            }
        } else if (gameInProgress && receivedCommand == remoteInput.commandJump) {
            game.jump();
            screenOutput.scheduleClear(DOOR_OPEN_TIME_MS);
        }
    }
}

//void printCurrentCode()
//{
////    Serial.print("Current code: ");
////    Serial.print(findButtonName(codeManager.getCode()[0]));
////    Serial.print(findButtonName(codeManager.getCode()[1]));
////    Serial.print(findButtonName(codeManager.getCode()[2]));
////    Serial.print(findButtonName(codeManager.getCode()[3]));
////    Serial.println();
//}

void factoryReset()
{
    codeManager.factoryReset();
    screenOutput.say(F("Zresetowane!"));
}
