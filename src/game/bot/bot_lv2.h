/**
 * Bot LV2 header file
 * BotLevel2 ke thua BotLevel1 (chien luoc MEDIUM)
 */

#pragma once

#include "bot_lv1.h"

class BotLevel2 : public BotLevel1 {
   protected:
    pII simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal,
                         const char botSymbol, const char playerSymbol);

   public:
    BotLevel2(const BotLevel& _level, const char& _symbol);
    ~BotLevel2() override;
    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};
