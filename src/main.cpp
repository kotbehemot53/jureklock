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
#include "StateManager.h"
#include "StateResetScheduler.h"

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
//      1. migrate to avr DA series?
//      2. 5V buck converter?
//      3. game freezing randomly on real hardware - why? and why didn't it freeze at one revision (see tags)?
//      4. u8g2 inside screenoutput? private inheritance of Task?

// generic variables and functions
const uint8_t numberButtons[10] = {BTN_0, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7, BTN_8, BTN_9};
unsigned const char defaultCode[4] = {BTN_1,BTN_1,BTN_1,BTN_1};
volatile struct RemoteInputReceivedData remoteInterruptData;
OneButtonTiny* resetBtn;
uint8_t receivedCommand;
Scheduler ts;

static void factoryReset();
void drawGame();
void handleListenForCodeToOpen();
void handleReceiveDigitToOpen();
void handleListenForCodeToChange();
void handleReceiveDigitToChangeCode();
void handleToggleGame();
void handleGameJump();

// game
Game game;

// input
RemoteInput remoteInput(
    BTN_ASTR,
    BTN_HASH,
    BTN_OK,
    BTN_UP,
    numberButtons
);

// output
StatusLEDOutput statusLedOutput(STATUS_LED_PIN, &ts);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
ScreenOutput screenOutput(&game, &u8g2);

// code
CodeManager codeManager(defaultCode);
CodeBuffer codeBuffer;

// lock
Lock lock(DOOR_PIN);

// state
StateManager stateManager(&codeBuffer);

// scheduling
StateResetScheduler stateResetScheduler(&ts, &stateManager, &screenOutput);
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
    // watchdog
    wdt_reset();

    // scheduling and button debounce
    ts.execute();
    resetBtn->tick();

    // handle game
    if (stateManager.isGameInProgress()) {
        drawGame();
        game.tick();
    }

    // handle input
    if (remoteInput.hasNewCommand(&remoteInterruptData)) {
        receivedCommand = remoteInput.getCommand();
        // start listening for code
        if (receivedCommand == remoteInput.commandEnterCodeToOpen) {
            handleListenForCodeToOpen();
        } else if (stateManager.isListeningForCodeToOpen() && remoteInput.isCommandNumeric(receivedCommand)) {
            handleReceiveDigitToOpen();
        } else if (receivedCommand == remoteInput.commandEnterCodeToChange && lock.isUnlocked()) {
            handleListenForCodeToChange();
        } else if (stateManager.isListeningForCodeToChange() && remoteInput.isCommandNumeric(receivedCommand)) {
            handleReceiveDigitToChangeCode();
        } else if (receivedCommand == remoteInput.commandStartGame) {
            handleToggleGame();
        } else if (stateManager.isGameInProgress() && receivedCommand == remoteInput.commandJump) {
            handleGameJump();
        }
    }
}

void handleGameJump()
{
    game.jump();

    stateResetScheduler.scheduleReset(DOOR_OPEN_TIME_MS);
}

void handleToggleGame()
{
    if (stateManager.isGameInProgress()) {
        stateResetScheduler.scheduleReset(1);
    } else {
        stateManager.startGame();

        game.initGame();

        stateResetScheduler.scheduleReset(DOOR_OPEN_TIME_MS);
    }
}

void handleReceiveDigitToChangeCode()
{
    screenOutput.drawStar(codeBuffer.getNumberOfDigitsReceived());

    stateManager.addDigit(receivedCommand);

    // full code received
    if (!stateManager.isListeningForCodeToChange()) {
        codeManager.saveCode(codeBuffer.getCode());

        screenOutput.say(F("Ustawiony!"));
        statusLedOutput.longBlink();
    }

    stateResetScheduler.scheduleReset(DOOR_OPEN_TIME_MS);
}

void handleListenForCodeToChange()
{
    stateManager.listenForCodeToChange();
    statusLedOutput.tripleBlink();
    screenOutput.say(F("Nowy kod?"));

    stateResetScheduler.scheduleReset(DOOR_OPEN_TIME_MS);
}

void handleReceiveDigitToOpen()
{
    screenOutput.drawStar(codeBuffer.getNumberOfDigitsReceived());

    stateManager.addDigit(receivedCommand);

    // full code received
    if (!stateManager.isListeningForCodeToOpen()) {
        if (codeManager.checkCode(codeBuffer.getCode())) {
            lockScheduler.unlockDoorAndScheduleLocking();

            screenOutput.say(F("Otwarte!"));
        } else {
            statusLedOutput.doubleBlink();
            screenOutput.say(F("Lipa, panie!"));
        }
    }

    stateResetScheduler.scheduleReset(DOOR_OPEN_TIME_MS);
}

void handleListenForCodeToOpen()
{
    stateManager.stopGame();

    stateManager.listenForCodeToOpen();
    statusLedOutput.shortBlink();

    screenOutput.say(F("Dawaj kod!"));

    stateResetScheduler.scheduleReset(DOOR_OPEN_TIME_MS);
}

void drawGame()
{
    if (game.isGameOver()) {
        screenOutput.drawGameOver();
        stateManager.stopGame();
    } else {
        screenOutput.drawGame();
    }
}

void factoryReset()
{
    codeManager.factoryReset();
    screenOutput.say(F("Zresetowane!"));
}
