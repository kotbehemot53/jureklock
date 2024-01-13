//
// Created by behemot on 13.01.2024.
//

#ifndef JUREKLOCK1_GAME_H
#define JUREKLOCK1_GAME_H

#define SLOWDOWN_FACTOR 20.0
#define INIT_JUMP_SPEED 2.2
#define GRAVITY 0.15

#include "Arduino.h"

class Game
{
private:
    static const int groundHeight = 30;
    static const byte obstacleCount = 10;

    byte mainCharacterSymbol = 43;
    byte obstacleSymbols[10] = {33, 62, 121, 56, 89, 112, 99, 90, 53, 101};

    unsigned long prevTickTime = 0;
    unsigned long prevSpawnTime = 0;
    unsigned long prevScoreTime = 0;
    unsigned long gameStartTime = 0;

    int score = 0;

    byte obstaclePositions[10] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127};
    int obstacleInternalPositions[10] = {127, 127, 127, 127, 127, 127, 127, 127, 127, 127};
    bool obstacleEnabled[10] = {true, false, false, false, false, false, false, false, false, false};

    byte mainCharacterPosition = 5;
    byte mainCharacterVerticalPosition = 30;
    double mainCharacterVerticalSpeed = 0;

    byte speed = 1;

    bool gameOver = false;

public:
    void initGame();
    void tick();
    byte getObstacleCount();
    byte getObstaclePosition(byte i);
    byte getObstacleSymbol(byte i);
    bool isObstacleEnabled(byte i);
    byte getMainCharacterPosition();
    byte getMainCharacterVerticalPosition();
    byte getMainCharacterSymbol();
    bool isGameOver();
    void jump();
    int getScore();
};

#endif //JUREKLOCK1_GAME_H
