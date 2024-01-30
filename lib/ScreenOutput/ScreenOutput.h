//
// Created by behemot on 28.01.2024.
//

#ifndef JUREKLOCK1_SCREENOUTPUT_H
#define JUREKLOCK1_SCREENOUTPUT_H

#include <Arduino.h>
#include "Game.h"
#include <U8g2lib.h>

#define _TASK_OO_CALLBACKS      // Support for dynamic callback method binding
#include <TaskSchedulerDeclarations.h>

class ScreenOutput : public Task
{
private:
    Game* game;
    U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C* u8g2;
    char gameScoreStr[100] = "Wynik: 0";

public:
    ScreenOutput(Scheduler* ts, Game* aGame, U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C* aU8g2);

    void say(const __FlashStringHelper *, byte height = 26);
    void say2Lines(const char *, const char *);
    void drawStar(byte);
    void clearScreen();
    void scheduleClear(int timeout);

    void drawGame();
    void drawGameOver();

    bool Callback();
};


#endif //JUREKLOCK1_SCREENOUTPUT_H
