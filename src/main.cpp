#include <Arduino.h>
#include <EEPROM.h>
//#include <../.pio/libdeps/Upload_UART/IRremote/src/TinyIRReceiver.hpp>
//#include <../.pio/libdeps/Upload_UART/OneButton/src/OneButtonTiny.h>
//#include <../.pio/libdeps/Upload_UART/arduino-timer/src/arduino-timer.h>
//#include <U8g2lib.h>
#include <TinyIRReceiver.hpp>
#include <OneButtonTiny.h>
#include <arduino-timer.h>
//#include <U8x8lib.h>
#include <U8g2lib.h>

#include <../lib/Lock/Lock.h>

//#define IR_RECEIVE_PIN 2
#define DOOR_PIN 8
#define NEG_RESET_PIN 9
#define STATUS_LED_PIN 6

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

// Define the dimension of the U8x8log window
#define U8LOG_WIDTH 16
#define U8LOG_HEIGHT 4

// TODO:
//      1. refactor (OOP)
//      2. add screen maybe?
//      3. add some silly game??
//      3. migrate to Attiny? avr DA series?
//      4. self-etched board?
//      5. 5V buck converter?

volatile struct TinyIRReceiverCallbackDataStruct sCallbackData;

unsigned char numberButtons[10] = {BTN_0, BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6, BTN_7, BTN_8, BTN_9};
bool isNumberButton(unsigned char command);
inline char* findButtonName(unsigned char code)
{
    switch (code) {
        case BTN_HASH:
            return "#";
        case BTN_ASTR:
            return "*";
        case BTN_0:
            return "0";
        case BTN_1:
            return "1";
        case BTN_2:
            return "2";
        case BTN_3:
            return "3";
        case BTN_4:
            return "4";
        case BTN_5:
            return "5";
        case BTN_6:
            return "6";
        case BTN_7:
            return "7";
        case BTN_8:
            return "8";
        case BTN_9:
            return "9";
        case BTN_DN:
            return "DOWN";
        case BTN_UP:
            return "UP";
        case BTN_L:
            return "LEFT";
        case BTN_R:
            return "RIGHT";
        case BTN_OK:
            return "OK";
    }
    return "";
}

// TODO: rename to CodeManager and move EEPROM storage there?
Lock lock;
OneButtonTiny* resetBtn;
auto timer = timer_create_default();

// TODO: all these to CodeManager?
short codeBufferPtr = -1;
bool listeningToOpen = false;
bool listeningToChangeCode = false;
unsigned char codeBuffer[4] = {LOCK_DEFAULT_DIGIT_0,LOCK_DEFAULT_DIGIT_1,LOCK_DEFAULT_DIGIT_2,LOCK_DEFAULT_DIGIT_3};
void printCurrentCode();
void saveCode();
void loadCode();
static void factoryReset();

// TODO: class Lock?
void* doorLockingTask;
int isDoorUnlocked();
void unlockDoor();
bool lockDoor(void*);
void unlockDoorAndScheduleLocking();


// TODO: class RemoteInput?
void stopListeningForCode();
void listenForCodeToOpen();
void listenForNewCode();

// TODO: class StatusLEDOutput?
bool statusLEDOn(void*);
bool statusLEDOff(void*);
void shortBlink();
void longBlink();
void doubleBlink();
void tripleBlink();

// U8x8 constructor for your display
//U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(U8X8_PIN_NONE, 19, 18);
//U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2();
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 19, 18);
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 19, 18);
//// Create a U8x8log object
//U8X8LOG u8x8log;
//// Allocate static memory for the U8x8log window
//uint8_t u8log_buffer[U8LOG_WIDTH*U8LOG_HEIGHT];

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

//    // Startup U8x8
//    u8x8.begin();
//    // Set a suitable font. This font will be used for U8x8log
//    u8x8.setFont(u8x8_font_chroma48medium8_r);
//    // Start U8x8log, connect to U8x8, set the dimension and assign the static memory
//    u8x8log.begin(u8x8, U8LOG_WIDTH, U8LOG_HEIGHT, u8log_buffer);
//    // Set the U8x8log redraw mode
//    u8x8log.setRedrawMode(1);		// 0: Update screen with newline, 1: Update screen for every char

    u8g2.begin();
}

static void factoryReset()
{
    codeBuffer[0] = LOCK_DEFAULT_DIGIT_0;
    codeBuffer[1] = LOCK_DEFAULT_DIGIT_1;
    codeBuffer[2] = LOCK_DEFAULT_DIGIT_2;
    codeBuffer[3] = LOCK_DEFAULT_DIGIT_3;
    saveCode();
//    u8x8log.print("\nKOD ZRESETOWANY\n");
//    u8g2.clearBuffer();
//    u8g2.setFont(u8g2_font_ncenB14_tr);
//    u8g2.drawStr(0,20,"\nKOD ZRESETOWANY\n");
//    u8g2.sendBuffer();

    Serial.println("Factory code reset done.");
    printCurrentCode();
}

void loop() {
    timer.tick();
    resetBtn->tick();


//    u8g2.clearBuffer();
//    u8g2.setFont(u8g2_font_ncenB14_tr);
//    u8g2.drawStr(0,20,"dawaj kod!\n");
//    u8g2.sendBuffer();


    if (sCallbackData.justWritten) {
        sCallbackData.justWritten = false;

        if (sCallbackData.Flags != IRDATA_FLAGS_IS_REPEAT) {

            // TODO: debug method?
            Serial.print(F("Address=0x"));
            Serial.print(sCallbackData.Address, HEX);
            Serial.print(F(" Command=0x"));
            Serial.print(sCallbackData.Command, HEX);
            Serial.print(F(" Btn="));
            Serial.print(findButtonName(sCallbackData.Command));

            if (sCallbackData.Flags == IRDATA_FLAGS_PARITY_FAILED) {
                Serial.print(F(" Parity failed"));
            }
            Serial.println();

            if (sCallbackData.Command == BTN_ASTR) {
//                u8x8log.print("dawaj kod!\n");


                u8g2.firstPage();
                do {
                    u8g2.setFont(u8g2_font_ncenB14_tr);
                    u8g2.drawStr(0,24,"Dawaj kod!");
                } while ( u8g2.nextPage() );
                u8g2.setCursor(0, 15);

                listenForCodeToOpen();
                shortBlink();
                // TODO: introduce keyboard class?
            } else if (listeningToOpen && codeBufferPtr < 4 && isNumberButton(sCallbackData.Command)) {
                codeBuffer[codeBufferPtr++] = sCallbackData.Command;
                // Print a number on the U8x8log window
                // The display will be refreshed
//                u8x8log.print("*");

//                if (codeBufferPtr == 0)
//                u8g2.setFont(u8g2_font_ncenB14_tr);
//                    u8g2.drawStr(codeBufferPtr * 10,24,"*");
                u8g2.firstPage();
                do {
                    u8g2.print("*");
                } while ( u8g2.nextPage() );

                if (codeBufferPtr >= 4) {
//                    u8x8log.print("\n");

                    stopListeningForCode();

                    if(lock.checkCode(codeBuffer)) {
//                        u8x8log.print("otwarte!\n");
                        unlockDoorAndScheduleLocking();
                    } else {
//                        u8x8log.print("lipa, panie!\n");
                        doubleBlink();
                        Serial.println("Code check failed.");
                    }
                }
            } else if (isDoorUnlocked() && sCallbackData.Command == BTN_HASH) {
//                u8x8log.print("dawaj nowy kod!\n");
                listenForNewCode();
                tripleBlink();
            } else if (listeningToChangeCode && codeBufferPtr < 4 && isNumberButton(sCallbackData.Command)) {
//                u8x8log.print("*");
                codeBuffer[codeBufferPtr++] = sCallbackData.Command;
                if (codeBufferPtr >= 4) {
//                    u8x8log.print("\nzmienione!\n");
                    stopListeningForCode();

                    saveCode();
                    Serial.println("Code set.");
                    printCurrentCode();
                    longBlink();
                }
            }
        }
    }

//    // Print a new line, scroll the text window content if required
//    // Refresh the screen
//    u8x8log.print("\n");

}

//char* findButtonName(unsigned char code)

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
    Serial.print("Current code: ");
    Serial.print(findButtonName(lock.getCode()[0]));
    Serial.print(findButtonName(lock.getCode()[1]));
    Serial.print(findButtonName(lock.getCode()[2]));
    Serial.print(findButtonName(lock.getCode()[3]));
    Serial.println();
}

void unlockDoorAndScheduleLocking()
{
    timer.cancel(doorLockingTask);
    unlockDoor();
    doorLockingTask = timer.in(DOOR_OPEN_TIME_MS, lockDoor);
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
