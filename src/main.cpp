#include <Arduino.h>
#include <EEPROM.h>
#include <../.pio/libdeps/uno/IRremote/src/TinyIRReceiver.hpp>
#include <../.pio/libdeps/uno/OneButton/src/OneButtonTiny.h>
#include <../.pio/libdeps/uno/arduino-timer/src/arduino-timer.h>
#include <../lib/Lock/Lock.h>

//#define IR_RECEIVE_PIN 2
#define DOOR_PIN 8
#define NEG_RESET_PIN 9
#define STATUS_LED_PIN 10

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

#define DOOR_OPEN_TIME_MS 10000 // 10 s

volatile struct TinyIRReceiverCallbackDataStruct sCallbackData;

unsigned char numberButtons[10] = {BTN_0, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7, BTN_8, BTN_9};
bool isNumberButton(unsigned char command);

Lock lock;
OneButtonTiny* resetBtn;
auto timer = timer_create_default();

short codeBufferPtr = -1;
bool listeningToOpen = false;
bool listeningToChangeCode = false;
unsigned char codeBuffer[4] = {LOCK_DEFAULT_DIGIT_0,LOCK_DEFAULT_DIGIT_1,LOCK_DEFAULT_DIGIT_2,LOCK_DEFAULT_DIGIT_3};
void printCurrentCode();
void saveCode();
void loadCode();

void* doorClosingTask;
int isDoorUnlocked();
void unlockDoor();
bool lockDoor(void*);

void stopListeningForCode();
void listenForCodeToOpen();
void listenForNewCode();

static void factoryReset();

bool statusLEDOn(void*);
bool statusLEDOff(void*);
void shortBlink();
void longBlink();
void doubleBlink();
void tripleBlink();

void unlockDoorAndScheduleLocking();

void setup() {
    pinMode(DOOR_PIN, OUTPUT);
    pinMode(STATUS_LED_PIN, OUTPUT);

    digitalWrite(DOOR_PIN, LOW);
    digitalWrite(STATUS_LED_PIN, LOW);

    resetBtn = new OneButtonTiny(NEG_RESET_PIN);
    resetBtn->attachClick(factoryReset);

    initPCIInterruptForTinyReceiver();

    Serial.begin(115200);

    Serial.println("Hullo");

    // save a default code if non has been set yet
    if (
        !isNumberButton(EEPROM.read(0)) ||
        !isNumberButton(EEPROM.read(1)) ||
        !isNumberButton(EEPROM.read(2)) ||
        !isNumberButton(EEPROM.read(3))
    ) {
        factoryReset();
    }

    // initialize saved code
    loadCode();

    // print the current code
    printCurrentCode();
}

static void factoryReset()
{
    codeBuffer[0] = LOCK_DEFAULT_DIGIT_0;
    codeBuffer[1] = LOCK_DEFAULT_DIGIT_1;
    codeBuffer[2] = LOCK_DEFAULT_DIGIT_2;
    codeBuffer[3] = LOCK_DEFAULT_DIGIT_3;
    saveCode();
    Serial.println("Factory code reset done.");
    printCurrentCode();
}

void loop() {
    timer.tick();
    resetBtn->tick();

    if (sCallbackData.justWritten) {
        sCallbackData.justWritten = false;

        if (sCallbackData.Flags != IRDATA_FLAGS_IS_REPEAT) {

            // TODO: debug method?
            Serial.print(F("Address=0x"));
            Serial.print(sCallbackData.Address, HEX);
            Serial.print(F(" Command=0x"));

            Serial.print(sCallbackData.Command, HEX);
            if (sCallbackData.Flags == IRDATA_FLAGS_PARITY_FAILED) {
                Serial.print(F(" Parity failed"));
            }
            Serial.println();

            if (sCallbackData.Command == BTN_ASTR) {
                listenForCodeToOpen();
                shortBlink();
                // TODO: introduce keyboard class?
            } else if (listeningToOpen && codeBufferPtr < 4 && isNumberButton(sCallbackData.Command)) {
                codeBuffer[codeBufferPtr++] = sCallbackData.Command;
                if (codeBufferPtr >= 4) {
                    stopListeningForCode();

                    if(lock.checkCode(codeBuffer)) {
                        unlockDoorAndScheduleLocking();
                    } else {
                        doubleBlink();
                        Serial.println("Code check failed.");
                    }
                }
            } else if (isDoorUnlocked() && sCallbackData.Command == BTN_HASH) {
                listenForNewCode();
                tripleBlink();
            } else if (listeningToChangeCode && codeBufferPtr < 4 && isNumberButton(sCallbackData.Command)) {
                codeBuffer[codeBufferPtr++] = sCallbackData.Command;
                if (codeBufferPtr >= 4) {
                    stopListeningForCode();

                    saveCode();
                    Serial.println("Code set.");
                    printCurrentCode();
                    longBlink();
                }
            }
        }
    }

}

void listenForNewCode()
{
    codeBufferPtr = 0;
    listeningToOpen = false;
    listeningToChangeCode = true;
    Serial.println("Listening for new code");
}

void listenForCodeToOpen()
{
    codeBufferPtr = 0;
    listeningToOpen = true;
    listeningToChangeCode = false;
    Serial.println("Listening for code to open");
}

void stopListeningForCode()
{
    codeBufferPtr = -1;
    listeningToOpen = false;
    listeningToChangeCode = false;
}

bool isNumberButton(unsigned char command)
{
    for (unsigned char numberButton : numberButtons) {
        if (numberButton == command) {
            return true;
        }
    }

    return false;
}

void saveCode()
{
    codeBufferPtr = -1;
    for (short i = 0; i < 4; i++) {
        EEPROM.write(i, codeBuffer[i]);
    }
    // TODO: maybe the lock should save it in memory and retrieve it? or not?
    lock.setCode(codeBuffer);
}

void loadCode()
{
    codeBufferPtr = -1;
    for (short i = 0; i < 4; i++) {
        codeBuffer[i] = EEPROM.read(i);
    }
    lock.setCode(codeBuffer);
}

void handleReceivedTinyIRData(uint8_t aAddress, uint8_t aCommand, uint8_t aFlags)
{
    sCallbackData.Command = aCommand;
    sCallbackData.Flags = aFlags;
    sCallbackData.justWritten = true;
}

void printCurrentCode()
{
    Serial.print("Current code: 0x");
    Serial.print(lock.getCode()[0], HEX);
    Serial.print(" 0x");
    Serial.print(lock.getCode()[1], HEX);
    Serial.print(" 0x");
    Serial.print(lock.getCode()[2], HEX);
    Serial.print(" 0x");
    Serial.print(lock.getCode()[3], HEX);
    Serial.println();
}

void unlockDoorAndScheduleLocking()
{
    timer.cancel(doorClosingTask);
    unlockDoor();
    doorClosingTask = timer.in(DOOR_OPEN_TIME_MS, lockDoor);
}

int isDoorUnlocked() { return digitalRead(DOOR_PIN); }
bool lockDoor(void*) {
    digitalWrite(DOOR_PIN, LOW);
    statusLEDOff(NULL);
    Serial.println("Door locked.");

    return false;
}
void unlockDoor()
{
    digitalWrite(DOOR_PIN, HIGH);
    statusLEDOn(NULL);
    Serial.println("Door unlocked.");
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
