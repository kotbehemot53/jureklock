#include <Arduino.h>
#include <EEPROM.h>
#include <../.pio/libdeps/uno/IRremote/src/TinyIRReceiver.hpp>
#include <../lib/Lock/Lock.h>

//#define IR_RECEIVE_PIN 2

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

volatile struct TinyIRReceiverCallbackDataStruct sCallbackData;

unsigned char numberButtons[10] = {BTN_0, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7, BTN_8, BTN_9};
bool isNumberButton(unsigned char command);

Lock lock;
short codeBufferPtr = -1;
unsigned char codeBuffer[4] = {LOCK_DEFAULT_DIGIT_0,LOCK_DEFAULT_DIGIT_1,LOCK_DEFAULT_DIGIT_2,LOCK_DEFAULT_DIGIT_3};
void printCurrentCode();
void saveCode();
void loadCode();

void setup() {
//    pinMode(LED_BUILTIN, OUTPUT);

    initPCIInterruptForTinyReceiver();

    Serial.begin(115200);

    Serial.println("Hullo");

    // save a default code if non has been set yet
    if (255 == EEPROM.read(0) && 255 == EEPROM.read(1) && 255 == EEPROM.read(2) && 255 == EEPROM.read(3)) {
        saveCode();
    }

    // initialize saved code
    loadCode();

    // print the current code
    printCurrentCode();
}

void loop() {
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

//            EEPROM.write(0, sCallbackData.Command);
            if (sCallbackData.Command == BTN_ASTR) {
                // initialize inputting code
                codeBufferPtr = 0;
                Serial.println("Listening");
                // TODO: introduce keyboard class?
            } else if (codeBufferPtr >= 0 && codeBufferPtr < 4 && isNumberButton(sCallbackData.Command)) {
                codeBuffer[codeBufferPtr++] = sCallbackData.Command;
                if (codeBufferPtr >= 4) {
                    codeBufferPtr = -1;

                    // TODO: lock.checkCode(codeBuffer) + msg instead of setting it here
                    saveCode();
                    Serial.println("Code set.");
                    printCurrentCode();
                }
            }
        }
    }

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