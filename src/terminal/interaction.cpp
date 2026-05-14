#include "interaction.h"
#include "../utils/logger.h"
#include <format>
#include <iostream>
#include <limits>

std::streambuf* initInteraction(const RunConfig& config) {
    std::streambuf* cin_backup = nullptr;
    if (!config.interactive && !config.input_file.empty()) {
        GameInteraction::global_file_in.open(config.input_file);
        if (GameInteraction::global_file_in.is_open()) {
            cin_backup = std::cin.rdbuf();
            std::cin.rdbuf(GameInteraction::global_file_in.rdbuf());
            GameLogger::log(std::format("redirected cin to: {}", config.input_file));
        } else {
            GameLogger::log("failed to open input file, using console.", GameLogger::Level::ERROR);
        }
    }
    return cin_backup;
}

void closeInteraction(std::streambuf* cin_backup) {
    if (cin_backup) {
        std::cin.rdbuf(cin_backup);
        GameLogger::log("fallback using 'std::cin' input stream.");
    }
    if (GameInteraction::global_file_in.is_open())
        GameInteraction::global_file_in.close();
}

bool validateInput(std::string& input) {
    if (input.empty()) return false;
    for (char c : input)
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

bool getInput(int* val) {
    if (!val) return false;
    std::string input;
    if (!(std::cin >> input)) return false;
    if (!validateInput(input)) return false;
    try {
        *val = std::stoi(input);
        return true;
    } catch (...) { return false; }
}

bool selectSize(int* size) {
    if (!size) return false;
    int tmp = 0;
    if (getInput(&tmp) && tmp >= 3 && tmp <= BOARD_N_MAX) {
        *size = tmp;
        return true;
    }
    return false;
}

bool selectGoal(int* goal, const int size) {
    if (!goal) return false;
    int tmp = 0;
    if (getInput(&tmp) && tmp >= 3 && tmp <= size) {
        *goal = tmp;
        return true;
    }
    return false;
}

bool selectGameMode(GameMode* mode) {
    int tmp = 0;
    if (getInput(&tmp) && tmp >= 1 && tmp <= 3) {
        *mode = static_cast<GameMode>(tmp - 1);
        return true;
    }
    return false;
}

bool selectBotLevel(BotLevel* levels, const int index) {
    if (!levels || index < 0 || index > 1) return false;
    int tmp = 0;
    if (getInput(&tmp) && tmp >= 1 && tmp <= 3) {
        levels[index] = static_cast<BotLevel>(tmp - 1);
        return true;
    }
    return false;
}

bool getPlayerMove(int* row, int* col) {
    if (!row || !col) return false;
    return getInput(row) && getInput(col);
}
