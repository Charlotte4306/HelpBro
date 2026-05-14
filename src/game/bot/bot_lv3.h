/**
 * Bot LV3 header file
 */

#pragma once

#include "../setup.h"
#include "bot_base.h"

class BotLevel3 : public Bot {
   protected:
    pII hard_level(char board[][BOARD_N_MAX], const int size, const int goal,
                   const char botSymbol, const char playerSymbol);

   public:
    BotLevel3(const BotLevel& _level, const char& _symbol);
    ~BotLevel3() override;
    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};
