#pragma once
#include "../setup.h"

/**
 * Base class cho tất cả Bot.
 * Các BotLevel1/2/3 kế thừa class này.
 */
class Bot {
   protected:
    BotLevel level;
    char symbol;

   public:
    Bot(const BotLevel& _level, const char& _symbol)
        : level(_level), symbol(_symbol) {}

    virtual ~Bot() {}

    virtual pII getMove(char board[][BOARD_N_MAX], const int size, const int goal) = 0;

    BotLevel getLevel() const { return level; }
    char getSymbol() const { return symbol; }
};
