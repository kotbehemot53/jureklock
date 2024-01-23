#include <Arduino.h>
#include <avr/wdt.h>

#define IR_RECEIVE_PIN 2

#include <TinyIRReceiver.hpp>
#include <OneButtonTiny.h>
#include <U8g2lib.h>
#include <TaskScheduler.h>

#include "CodeManager.h"
#include "Lock.h"
#include "Game.h"
#include "RemoteInput.h"
#include "CodeBuffer.h"

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
//      4. after manual game turn-off, pressing asterisk causes permanent status light, instead of a blink - why???
//      5. game freezing randomly on real hardware - why? and why didn't it freeze at one revision (see tags)?


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

// TODO: what about these:
OneButtonTiny* resetBtn;
uint8_t receivedCommand;

// TODO: class StatusLEDOutput?
bool statusLEDOn(void*);
bool statusLEDOff(void*);
void shortBlink();
void longBlink();
void doubleBlink();
void tripleBlink();

// TODO: class screen?
void screenSay(const __FlashStringHelper *, byte height = 26);
void screenSay2Lines(const char *, const char *);
void screenDrawStar(byte);
void screenScheduleClear(int timeout);
bool screenClear(void*);
void* screenClearingTask;
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

// TODO: debug class?
//void printCurrentCode();

// TODO: some orchestrator class?
void* doorLockingTask;
bool lockDoor(void*);
void unlockDoorAndScheduleLocking();

// game
Game game;
bool gameInProgress = false;
char gameScoreStr[100] = "Wynik: 0";

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

//    remoteInterruptData.Flags = aFlags;

//    remoteInput.setReceivedCommand(
//        aCommand,
//        aFlags == IRDATA_FLAGS_IS_REPEAT,
//        aFlags == IRDATA_FLAGS_PARITY_FAILED
//    );
}

void loop() {
    wdt_reset();

//    timer.tick();
    ts.execute();
    resetBtn->tick();

    // draw game
    if (gameInProgress) {
        // calculate score string
        sprintf(gameScoreStr, "Wynik: %d", game.getScore());

        if (game.isGameOver()) {
            // draw game over screen
            screenSay2Lines("Koniec!", gameScoreStr);

            gameInProgress = false;
        } else {
            // make it a separate screen method? which takes game as arg?
            u8g2.clearBuffer();
            // draw main character
            u8g2.setFont(u8g2_font_unifont_t_animals);
            u8g2.drawGlyph(game.getMainCharacterPosition(), game.getMainCharacterVerticalPosition(), game.getMainCharacterSymbol());

            // draw obstacles
            for (byte i = 0; i < game.getObstacleCount(); ++i) {
                if (game.isObstacleEnabled(i)) {
                    u8g2.drawGlyph(game.getObstaclePosition(i), 30, game.getObstacleSymbol(i));
                }
            }

            // draw score
            u8g2.setFont(u8g2_font_t0_11_mr );
            u8g2.drawStr(46,10,gameScoreStr);

            // draw ground
            u8g2.drawLine(0, 31, 127, 31);
            u8g2.sendBuffer();
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
            shortBlink();

            screenSay(F("Dawaj kod!"));
        } else if (codeBuffer.isListeningForCodeToOpen() && remoteInput.isCommandNumeric(receivedCommand)) {
            screenDrawStar(codeBuffer.getNumberOfDigitsReceived());

            codeBuffer.addDigit(receivedCommand);

            // full code received
            if (!codeBuffer.isListeningForCodeToOpen()) {
                if (codeManager.checkCode(codeBuffer.getCode())) {
                    unlockDoorAndScheduleLocking();

                    screenSay(F("Otwarte!"));
                } else {
                    doubleBlink();
                    screenSay(F("Lipa, panie!"));
//                        Serial.println("Code check failed.");
                }
            }
        } else if (receivedCommand == remoteInput.commandEnterCodeToChange && lock.isUnlocked()) {
//            Serial.println("Getting new code");

            codeBuffer.listenForCodeToChange();
            tripleBlink();

            screenSay(F("Nowy kod?"));
        } else if (codeBuffer.isListeningForCodeToChange() && remoteInput.isCommandNumeric(receivedCommand)) {
            screenDrawStar(codeBuffer.getNumberOfDigitsReceived());

            codeBuffer.addDigit(receivedCommand);

            // full code received
            if (!codeBuffer.isListeningForCodeToChange()) {
                codeManager.saveCode(codeBuffer.getCode());

                screenSay(F("Ustawiony!"));
//                    Serial.println("Code set.");
//                    printCurrentCode();
                longBlink();
            }
        } else if (receivedCommand == remoteInput.commandStartGame) {
            if (gameInProgress) {
                // TODO: move gameInProgress to game?
                gameInProgress = false;
                screenScheduleClear(1);
            } else {
                codeBuffer.stopListeningForCode();

                game.initGame();
                gameInProgress = true;

                //TODO:  separate const for that?
                screenScheduleClear(DOOR_OPEN_TIME_MS);
            }
        } else if (gameInProgress && receivedCommand == remoteInput.commandJump) {
            game.jump();
            screenScheduleClear(DOOR_OPEN_TIME_MS);
        }
    }
}

void screenDrawStar(byte starNo)
{
    const char* stars[] = {"*", "* *", "* * *", "* * * *"};
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_calblk36_tr);
    u8g2.drawStr(20,38,stars[starNo]);
    u8g2.sendBuffer();

    screenScheduleClear(DOOR_OPEN_TIME_MS);
}

void screenSay(const __FlashStringHelper* text, byte height)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_crox4hb_tr);
    u8g2.setCursor(0, height);
    u8g2.print(text);
    u8g2.sendBuffer();

    screenScheduleClear(DOOR_OPEN_TIME_MS);
}

void screenSay2Lines(const char* text, const char* text2)
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.drawStr(0,15,text);
    u8g2.drawStr(0,31,text2);
    u8g2.sendBuffer();

    screenScheduleClear(DOOR_OPEN_TIME_MS);
}

void screenScheduleClear(int timeout)
{
    timer.cancel(screenClearingTask);
    screenClearingTask = timer.in(timeout, screenClear);
}

bool screenClear(void*)
{
    u8g2.clearDisplay();
    gameInProgress = false;

    return false;
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

void unlockDoorAndScheduleLocking()
{
    timer.cancel(doorLockingTask);
    lock.unlock();
    statusLEDOn(NULL);
    doorLockingTask = timer.in(DOOR_OPEN_TIME_MS, lockDoor);
}

bool lockDoor(void*) {
    lock.lock();
    statusLEDOff(NULL);
    screenSay(F("Zamkniete!"));
//    Serial.println("Door locked.");

    return false;
}

bool statusLEDOn(void*)
{
    digitalWrite(STATUS_LED_PIN, HIGH);

    return false;
}

bool statusLEDOff(void*)
{
    digitalWrite(STATUS_LED_PIN, LOW);

    return false;
}

void shortBlink()
{
    statusLEDOn(NULL);
    timer.in(50, statusLEDOff);
}

void longBlink()
{
    statusLEDOn(NULL);
    timer.in(1000, statusLEDOff);
}

void doubleBlink()
{
    statusLEDOn(NULL);
    timer.in(100, statusLEDOff);
    timer.in(200, statusLEDOn);
    timer.in(300, statusLEDOff);
}

void tripleBlink()
{
    statusLEDOn(NULL);
    timer.in(100, statusLEDOff);
    timer.in(200, statusLEDOn);
    timer.in(300, statusLEDOff);
    timer.in(400, statusLEDOn);
    timer.in(500, statusLEDOff);
}

void factoryReset()
{
    codeManager.factoryReset();
    screenSay(F("Zresetowane!"));
}
