//
// Created by behemot on 13.01.2024.
//

#include "Game.h"

void Game::initGame()
{
//    this->obstacleCount = 1;
    for (byte i = 0; i < 10; ++i) {
        this->obstaclePositions[i] = 127;
        this->obstacleInternalPositions[i] = 127;
        if (i == 0) {
            this->obstacleEnabled[i] = false;
        } else {
            this->obstacleEnabled[i] = false;
        }
    }
    this->mainCharacterPosition = 5;
    this->gameStartTime = millis();
    this->prevSpawnTime = this->gameStartTime;
    this->prevScoreTime = this->gameStartTime;

    this->speed = 1;
    this->gameOver = false;
    this->mainCharacterVerticalPosition = groundHeight;
    this->mainCharacterVerticalSpeed = 0;

    this->score = 0;
}

void Game::tick()
{
    // calculate delta time
    double deltaTime;
    unsigned long currentTime = millis();
    if (currentTime < this->prevTickTime) {
        deltaTime = 10.0;
    } else {
        deltaTime = (currentTime - this->prevTickTime) * 1.0;
    }
    this->prevTickTime = currentTime;

    // TODO: increase speed

    // handle obstacle movement
    // TODO: use speed
    byte obstacleOffset = ceil( (deltaTime / SLOWDOWN_FACTOR) * speed );
    for (byte i = 0; i < this->obstacleCount; ++i) {
        if (this->obstacleEnabled[i]) {
            if (this->obstacleInternalPositions[i] > 0) {
                this->obstacleInternalPositions[i] -= obstacleOffset;
            } else {
                this->obstacleEnabled[i] = false;
                this->obstacleInternalPositions[i] = 127;
            }
            if (this->obstacleInternalPositions[i] >= 0 && this->obstacleInternalPositions[i] <= 127) {
                this->obstaclePositions[i] = this->obstacleInternalPositions[i];
            }

            if (this->obstacleInternalPositions[i] - this->mainCharacterPosition < 14 && this->mainCharacterVerticalPosition > 20) {
                this->gameOver = true;
            }
        } else {
            unsigned long spawnDelta = 1000;
            if (currentTime > this->prevSpawnTime) {
                spawnDelta = currentTime - this->prevSpawnTime;
            }
            if (random(1000,100000) < spawnDelta) {
                this->prevSpawnTime = currentTime;
                this->obstacleEnabled[i] = true;
            }
        }
    }

    // handle jumps
    // TODO: use speed
    this->mainCharacterVerticalPosition -= floor( (deltaTime / SLOWDOWN_FACTOR) * this->mainCharacterVerticalSpeed );
    this->mainCharacterVerticalSpeed -= GRAVITY;
    if (this->mainCharacterVerticalPosition >= groundHeight) {
        this->mainCharacterVerticalPosition = groundHeight;
        this->mainCharacterVerticalSpeed = 0;
    }

    // increment score every second
    if (!this->gameOver && ((currentTime < this->prevScoreTime) || (currentTime - this->prevScoreTime > 1000))) {
        ++score;
        this->prevScoreTime = currentTime;
    }
}

byte Game::getObstacleCount()
{
    return obstacleCount;
}

byte Game::getObstaclePosition(byte i)
{
    return this->obstaclePositions[i];
}

byte Game::getObstacleSymbol(byte i)
{
    return this->obstacleSymbols[i];
}

byte Game::getMainCharacterPosition()
{
    return this->mainCharacterPosition;
}

byte Game::getMainCharacterVerticalPosition()
{
    return this->mainCharacterVerticalPosition;
}

byte Game::getMainCharacterSymbol()
{
    return this->mainCharacterSymbol;
}

bool Game::isGameOver()
{
    return this->gameOver;
}

void Game::jump()
{
    if (this->mainCharacterVerticalPosition >= groundHeight) {
        this->mainCharacterVerticalSpeed = INIT_JUMP_SPEED;
    }
}

bool Game::isObstacleEnabled(byte i)
{
    return this->obstacleEnabled[i];
}

int Game::getScore()
{
    return this->score;
}
