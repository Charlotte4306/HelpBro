/**
 * Bot LV1 header file
 */

#pragma once

#include "../setup.h"
#include "bot_base.h"

class BotLevel1 : public Bot {
   protected:
    pII random_pick(char board[][BOARD_N_MAX], const int size);

   public:
    BotLevel1(const BotLevel& _level, const char& _symbol);
    ~BotLevel1() override;
    pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) override;
};
