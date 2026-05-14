#pragma once
#include "../utils/config.h"
#include <fstream>

namespace GameInteraction {
    static std::ifstream global_file_in;
}

std::streambuf* initInteraction(const RunConfig& config);
void closeInteraction(std::streambuf* cin_backup);
bool validateInput(std::string& input);
bool getInput(int* val);
bool selectSize(int* size);
bool selectGoal(int* goal, const int size);
bool selectGameMode(GameMode* mode);
bool selectBotLevel(BotLevel* levels, const int index);
bool getPlayerMove(int* row, int* col);
