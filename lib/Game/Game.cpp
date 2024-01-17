//
// Created by behemot on 13.01.2024.
//

#include "Game.h"

void Game::initGame()
{
    for (byte i = 0; i < this->obstacleCount; ++i) {
        this->obstaclePositions[i] = 127;
        this->obstacleInternalPositions[i] = 127;
        this->obstacleEnabled[i] = false;
    }
    this->mainCharacterPosition = 5;
    this->gameStartTime = millis();
    this->prevSpawnTime = this->gameStartTime;
    this->prevScoreTime = this->gameStartTime;

    this->speed = 1;
    this->gameOver = false;
    this->mainCharacterVerticalPosition = groundHeight;
    this->mainCharacterVerticalSpeed = 0;

    this->score = 0.0;
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

    // handle obstacle movement
    byte obstacleOffset = ceil( (deltaTime / (SLOWDOWN_FACTOR/speed)));// * speed );
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
    int deltaVert = floor( (deltaTime / (SLOWDOWN_FACTOR)) * this->mainCharacterVerticalSpeed );
    if (deltaVert > this->mainCharacterVerticalPosition) {
        deltaVert = this->mainCharacterVerticalPosition;
    }
    this->mainCharacterVerticalPosition -= deltaVert;
    this->mainCharacterVerticalSpeed -= GRAVITY * speed;
    if (this->mainCharacterVerticalPosition >= groundHeight) {
        this->mainCharacterVerticalPosition = groundHeight;
        this->mainCharacterVerticalSpeed = 0;
    }

    // increment score every 0.1 s
    if (!this->gameOver && ((currentTime < this->prevScoreTime) || (currentTime - this->prevScoreTime > 100))) {
        // TODO: this rounding is shit, use more accurate values, but still do the increments (with speed too?) because millis() overflows from time to time
        score += (currentTime - this->prevScoreTime) / 100.0;
        this->prevScoreTime = currentTime;

        speed += 0.005;
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
        this->mainCharacterVerticalSpeed = INIT_JUMP_SPEED * (1 + 0.33 * (speed - 1));
    }
}

bool Game::isObstacleEnabled(byte i)
{
    return this->obstacleEnabled[i];
}

int Game::getScore()
{
    return int(this->score);
}
