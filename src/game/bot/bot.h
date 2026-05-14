#pragma once
#include "../setup.h"

pII botMove(char board[][BOARD_N_MAX], const int size, const int goal, const char symbol, const BotLevel level);
pII random_pick(char board[][BOARD_N_MAX], const int size);
pII simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol);
pII hard_level(char board[][BOARD_N_MAX], const int size, const int goal, const char botSymbol, const char playerSymbol);
