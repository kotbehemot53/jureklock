#include <Arduino.h>
#include <EEPROM.h>
//#include <../.pio/libdeps/uno/IRremote/src/IRremote.hpp>
#include <../.pio/libdeps/uno/IRremote/src/TinyIRReceiver.hpp>

//#define IR_RECEIVE_PIN 8

#define BTN_UP 0x18
#define BTN_DN 0x52
#define BTN_L 0x8
#define BTN_R 0x5A
#define BTN_OK 0x1C
#define BTN_ASTR 0x16
#define BTN_HASH 0xD

volatile struct TinyIRReceiverCallbackDataStruct sCallbackData;

void setup() {
//    pinMode(LED_BUILTIN, OUTPUT);

//    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    initPCIInterruptForTinyReceiver();

    Serial.begin(115200);
    Serial.println("hullo");
    Serial.print("Current code: ");
    Serial.print(EEPROM.read(0));
    Serial.print(" ");
    Serial.print(EEPROM.read(1));
    Serial.print(" ");
    Serial.print(EEPROM.read(2));
    Serial.print(" ");
    Serial.print(EEPROM.read(3));
    Serial.print(" ");

    if (255 == EEPROM.read(0) && 255 == EEPROM.read(1) && 255 == EEPROM.read(2) && 255 == EEPROM.read(3)) {
        EEPROM.write(0, 0x45);
        EEPROM.write(1, 0x45);
        EEPROM.write(2, 0x45);
        EEPROM.write(3, 0x45);
    }
}

void loop() {
//    digitalWrite(LED_BUILTIN, HIGH);
//    delay(100);
//    digitalWrite(LED_BUILTIN, LOW);
//    delay(100);
//    digitalWrite(LED_BUILTIN, HIGH);
//    delay(100);
//    digitalWrite(LED_BUILTIN, LOW);
//    delay(1000);

//    if (IrReceiver.decode()) {
////        IrReceiver.decodeNEC();
//
//        IRData* received = IrReceiver.read();
//
////        Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
////        // USE NEW 3.x FUNCTIONS
//        Serial.println(EEPROM.read(0));
//        Serial.println(EEPROM.read(1));
//        Serial.println(EEPROM.read(2));
//        Serial.println(EEPROM.read(3));
//
//        IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
////        IrReceiver.printIRSendUsage(&Serial);   // Print the statement required to send this data
//        if (received->protocol == NEC) {
//            EEPROM.write(0, received->command);
//        }
//        IrReceiver.resume(); // Enable receiving of the next value
//    }

    if (sCallbackData.justWritten) {
        sCallbackData.justWritten = false;

        if (sCallbackData.Flags != IRDATA_FLAGS_IS_REPEAT) {
            Serial.println(EEPROM.read(0));

            Serial.print(F("Address=0x"));
            Serial.print(sCallbackData.Address, HEX);
            Serial.print(F(" Command=0x"));

            Serial.print(sCallbackData.Command, HEX);
//            if (sCallbackData.Flags == IRDATA_FLAGS_IS_REPEAT) {
//                Serial.print(F(" Repeat"));
//            }
            if (sCallbackData.Flags == IRDATA_FLAGS_PARITY_FAILED) {
                Serial.print(F(" Parity failed"));
            }
            Serial.println();

            EEPROM.write(0, sCallbackData.Command);
            Serial.println(EEPROM.read(0));
        }
    }

}

void handleReceivedTinyIRData(uint8_t aAddress, uint8_t aCommand, uint8_t aFlags)
{
    sCallbackData.Command = aCommand;
    sCallbackData.Flags = aFlags;
    sCallbackData.justWritten = true;
}