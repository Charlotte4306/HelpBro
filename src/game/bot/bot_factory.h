/**
 * Bot Factory header file
 */

#pragma once

#include "../setup.h"
#include "bot_base.h"

class BotFactory {
   public:
    static Bot* createBot(BotLevel level, const char& symbol);
};
