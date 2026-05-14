#include "renderer.h"
#include "../utils/config.h"
#include <iostream>
#include <string>

void clearScreen() {
    std::cout << "\n\x1B[2J\x1B[H\n";
}

void showSelectMenu(SelectType selectType) {
    const std::string RESET   = "\033[0m";
    const std::string TITLE_C = "\033[1;96m";
    const std::string PROMPT_C= "\033[1;97m";
    const std::string OPT_C   = "\033[1;93m";

    switch (selectType) {
    case SelectType::TITLE_UI:
        std::cout << TITLE_C
                  << "===============================================\n"
                  << " >>> Tic-tac-toe [Console v" << VERSION << "] <<<\n"
                  << "===============================================\n\n" << RESET;
        break;
    case SelectType::SIZE_UI:
        std::cout << PROMPT_C << "> Enter board size N (3 <= N <= " << BOARD_N_MAX << "): \n"
                  << OPT_C << "Choice: " << RESET;
        break;
    case SelectType::GOAL_UI:
        std::cout << PROMPT_C << "> Enter win goal (3 <= goal <= ";
        break;
    case SelectType::GAME_MODE_UI:
        std::cout << PROMPT_C << "> Select game mode:\n"
                  << OPT_C
                  << "  (1) PvP - Player vs Player\n"
                  << "  (2) PvE - Player vs Bot\n"
                  << "  (3) EvE - Bot vs Bot\n"
                  << "Choice: " << RESET;
        break;
    case SelectType::BOT_LEVEL_UI:
        std::cout << PROMPT_C << "> Select bot difficulty: "
                  << OPT_C << "(1) EASY | (2) MEDIUM | (3) HARD\n"
                  << "Choice: " << RESET;
        break;
    case SelectType::PLAYER_UI:
        std::cout << PROMPT_C << "> Enter your move (row col): " << RESET;
        break;
    case SelectType::MUL_BOT_LEVEL_UI:
        std::cout << PROMPT_C << "> Select difficulty for Bot 1 and Bot 2 (e.g., 1 3): \n"
                  << OPT_C << "Choice: " << RESET;
        break;
    default: break;
    }
}

void displayBoard(const char board[][BOARD_N_MAX], const int size, const GameSetup* setup, int turns) {
    std::string sideInfo[5];
    int infoCount = 0;

    if (setup) {
        sideInfo[infoCount++] = "Version : " + VERSION;
        sideInfo[infoCount++] = "Size    : " + std::to_string(size) + "x" + std::to_string(size);
        sideInfo[infoCount++] = "Goal    : " + std::to_string(setup->goal);
        std::string modeStr = (setup->mode == GameMode::PVP) ? "PvP" :
                              (setup->mode == GameMode::PVE) ? "PvE" : "EvE";
        if (setup->mode == GameMode::PVE) {
            const char* bls[] = {"EASY", "MEDIUM", "HARD"};
            modeStr += " (Bot: " + std::string(bls[static_cast<int>(setup->levels[1])]) + ")";
        }
        sideInfo[infoCount++] = "Mode    : " + modeStr;
        if (turns >= 0) sideInfo[infoCount++] = "Turns   : " + std::to_string(turns);
    }

    const std::string RESET   = "\033[0m";
    const std::string BORDER  = "\033[97m";
    const std::string INFO    = "\033[96m";
    const std::string COLOR_X = "\033[1;32m";
    const std::string HL_X    = "\033[42;1;97m";
    const std::string COLOR_O = "\033[1;31m";
    const std::string HL_O    = "\033[41;1;97m";

    int infoIdx = 0;
    auto printRowEnd = [&](int padding = 0) {
        std::cout << std::string(padding, ' ');
        if (infoIdx < infoCount)
            std::cout << " | " << INFO << sideInfo[infoIdx++] << RESET;
        std::cout << "\n";
    };

    std::cout << "\n  ";
    for (int c = 0; c < size; ++c)
        std::cout << BORDER << (c < 10 ? "  " : " ") << c << " " << RESET;
    printRowEnd(1);

    std::cout << BORDER << "  +";
    for (int c = 0; c < size; ++c) std::cout << "---+";
    std::cout << RESET;
    printRowEnd(0);

    for (int r = 0; r < size; ++r) {
        std::cout << BORDER << (r < 10 ? " " : "") << r << " |" << RESET;
        for (int c = 0; c < size; ++c) {
            char sym = board[r][c];
            bool isLastX = setup && setup->lastX.first == r && setup->lastX.second == c;
            bool isLastO = setup && setup->lastO.first == r && setup->lastO.second == c;

            if (sym == 'X')
                std::cout << (isLastX ? HL_X : COLOR_X) << " X " << RESET << BORDER << "|" << RESET;
            else if (sym == 'O')
                std::cout << (isLastO ? HL_O : COLOR_O) << " O " << RESET << BORDER << "|" << RESET;
            else
                std::cout << " " << sym << " " << BORDER << "|" << RESET;
        }
        printRowEnd(0);

        std::cout << BORDER << "  +";
        for (int c = 0; c < size; ++c) std::cout << "---+";
        std::cout << RESET;
        printRowEnd(0);
    }
    std::cout << "\n";
}

void showPlayer(int player, bool is_bot) {
    std::string pColor = (player == 1) ? "\033[1;32m" : "\033[1;31m";
    std::string sym    = (player == 1) ? " (X)" : " (O)";
    std::string label  = is_bot ? "Bot" : "Player";
    std::cout << "\033[1;36m>> \033[0m" << pColor << label << " " << player << sym << "\033[0m"
              << (is_bot ? "\033[3m is thinking...\033[0m\n" : " \033[1;33m's turn\033[0m\n");
}

void showMove(const int row, const int col) {
    std::cout << "  \033[1;32m[*]\033[0m Move placed at (\033[1;33m"
              << row << "\033[0m, \033[1;33m" << col << "\033[0m)\n";
}

void showInvalidMove() {
    std::cout << "  \033[1;41;37m [!] Invalid move! Please try again. \033[0m\n";
}

void showResult(const int winner, const bool is_bot) {
    std::cout << "\n\033[1;33m========================================\033[0m\n";
    if (winner == DRAW_RESULT) {
        std::cout << "\033[1;35m  IT'S A DRAW!  \033[0m\n";
    } else {
        std::string label  = is_bot ? "Bot" : "Player";
        std::string sym    = (winner == 0) ? " (X)" : " (O)";
        std::string color  = (winner == 0) ? "\033[1;32m" : "\033[1;31m";
        std::cout << color << "  " << label << " " << winner + 1 << sym << " WINS!  \033[0m\n";
    }
    std::cout << "\033[1;33m========================================\033[0m\n\n";
}

void printResult(const GameResult& gameResult) {
    std::cout << gameResult.winner << " " << gameResult.turns << "\n";
}
