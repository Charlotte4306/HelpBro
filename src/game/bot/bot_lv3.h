/**
 * Bot LV3 header file
 * BotLevel3 ke thua BotLevel2 (chien luoc HARD)
 */

#pragma once

#include "bot_lv2.h"

class BotLevel3 : public BotLevel2 {
   protected:
    pII hard_level(char board[][BOARD_N_MAX], const int size, const int goal,
                   const char botSymbol, const char playerSymbol);

   public:
    BotLevel3(const BotLevel& _level, const char& _symbol);
    ~BotLevel3() override;
    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};
