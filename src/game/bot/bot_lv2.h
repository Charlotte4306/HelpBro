/**
 * Bot LV2 header file
 */

#pragma once

#include "../setup.h"
#include "bot_base.h"

class BotLevel2 : public Bot {
   protected:
    pII simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal,
                         const char botSymbol, const char playerSymbol);

   public:
    BotLevel2(const BotLevel& _level, const char& _symbol);
    ~BotLevel2() override;
    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};
