//
// Created by behemot on 28.01.2024.
//

#include "ScreenOutput.h"

ScreenOutput::ScreenOutput()
{
}

ScreenOutput::ScreenOutput(Game *aGame, U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C *aU8g2)
{
    this->game = aGame;
    this->u8g2 = aU8g2;
}

void ScreenOutput::say(const __FlashStringHelper *text, byte height)
{
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_crox4hb_tr);
    u8g2->setCursor(0, height);
    u8g2->print(text);
    u8g2->sendBuffer();
}

void ScreenOutput::say2Lines(const char *text, const char *text2)
{
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_helvB12_tr);
    u8g2->drawStr(0,15,text);
    u8g2->drawStr(0,31,text2);
    u8g2->sendBuffer();
}

void ScreenOutput::drawStar(byte starNo)
{
    const char* stars[] = {"*", "* *", "* * *", "* * * *"};
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_calblk36_tr);
    u8g2->drawStr(20,38,stars[starNo]);
    u8g2->sendBuffer();
}

void ScreenOutput::clearScreen()
{
    u8g2->clearDisplay();
}

void ScreenOutput::drawGame()
{
    // calculate score string
    sprintf(this->gameScoreStr, "Wynik: %d", game->getScore());

    // make it a separate screen method? which takes game as arg?
    u8g2->clearBuffer();
    // draw main character
    u8g2->setFont(u8g2_font_unifont_t_animals);
    u8g2->drawGlyph(game->getMainCharacterPosition(), game->getMainCharacterVerticalPosition(), game->getMainCharacterSymbol());

    // draw obstacles
    for (byte i = 0; i < game->getObstacleCount(); ++i) {
        if (game->isObstacleEnabled(i)) {
            u8g2->drawGlyph(game->getObstaclePosition(i), 30, game->getObstacleSymbol(i));
        }
    }

    // draw score
    u8g2->setFont(u8g2_font_t0_11_mr );
    u8g2->drawStr(46,10,this->gameScoreStr);

    // draw ground
    u8g2->drawLine(0, 31, 127, 31);
    u8g2->sendBuffer();
}

void ScreenOutput::drawGameOver()
{
    this->say2Lines("Koniec!", this->gameScoreStr);
}
