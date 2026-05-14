#include "engine.h"
#include "logic.h"
#include "bot/bot.h"
#include "../terminal/interaction.h"
#include "../terminal/renderer.h"
#include "../utils/logger.h"
#include "../utils/helper.h"
#include <format>
#include <thread>
#include <chrono>
#include <limits>
#include <iostream>

void startGame(const RunConfig& config, GameSetup& gameSetup) {
    if (config.interactive) clearScreen();
    if (config.interactive) showSelectMenu(SelectType::TITLE_UI);

    do {
        if (config.interactive) showSelectMenu(SelectType::SIZE_UI);
    } while (!selectSize(&gameSetup.size));

    do {
        if (config.interactive) {
            showSelectMenu(SelectType::GOAL_UI);
            std::cout << gameSetup.size << "):\033[0m\n\033[1;33mChoice: \033[0m";
        }
    } while (!selectGoal(&gameSetup.goal, gameSetup.size));

    do {
        if (config.interactive) showSelectMenu(SelectType::GAME_MODE_UI);
    } while (!selectGameMode(&gameSetup.mode));

    if (gameSetup.mode == GameMode::PVE) {
        do {
            if (config.interactive) showSelectMenu(SelectType::BOT_LEVEL_UI);
        } while (!selectBotLevel(gameSetup.levels, 1));
    }

    if (gameSetup.mode == GameMode::EVE) {
        do {
            if (config.interactive) showSelectMenu(SelectType::MUL_BOT_LEVEL_UI);
        } while (!(selectBotLevel(gameSetup.levels, 0) && selectBotLevel(gameSetup.levels, 1)));
    }

    initBoard(gameSetup.board, gameSetup.size);
}

GameResult playGame(const RunConfig& config, GameSetup& gameSetup) {
    GameResult result;
    int currentPlayer = 0;
    char symbols[2] = {'X', 'O'};
    int turns = 0;

    while (true) {
        if (config.interactive) {
            clearScreen();
            displayBoard(gameSetup.board, gameSetup.size, &gameSetup, turns);
        }

        bool isBot = (gameSetup.mode == GameMode::EVE) ||
                     (gameSetup.mode == GameMode::PVE && currentPlayer == 1);

        if (config.interactive) showPlayer(currentPlayer + 1, isBot);

        int row = -1, col = -1;

        if (isBot) {
            std::string label = std::format("Bot {} ({})", currentPlayer + 1, symbols[currentPlayer]);
            pII point = measureExecutionTime(label, [&]() {
                return botMove(gameSetup.board, gameSetup.size, gameSetup.goal,
                               symbols[currentPlayer], gameSetup.levels[currentPlayer]);
            }, TIME_ENABLED);

            row = point.first;
            col = point.second;

            if (!isValidMove(gameSetup.board, gameSetup.size, row, col)) {
                GameLogger::log(std::format("WARNING: Bot {} invalid move ({},{}) - fallback to random.",
                                currentPlayer + 1, row, col), GameLogger::Level::WARNING);
                auto fb = random_pick(gameSetup.board, gameSetup.size);
                row = fb.first; col = fb.second;
            }

            if (config.interactive)
                std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
        } else {
            bool validMove = false;
            while (!validMove) {
                if (config.interactive) showSelectMenu(SelectType::PLAYER_UI);
                if (getPlayerMove(&row, &col) && isValidMove(gameSetup.board, gameSetup.size, row, col)) {
                    validMove = true;
                } else {
                    if (config.interactive) showInvalidMove();
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }
        }

        makeMove(gameSetup.board, row, col, symbols[currentPlayer]);
        turns++;

        if (symbols[currentPlayer] == 'X') gameSetup.lastX = {row, col};
        else                               gameSetup.lastO = {row, col};

        if (config.interactive) {
            showMove(row, col);
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }

        if (checkWin(gameSetup.board, gameSetup.size, symbols[currentPlayer], gameSetup.goal)) {
            result = {currentPlayer, isBot, turns};
            break;
        }
        if (checkDraw(gameSetup.board, gameSetup.size)) {
            result = {DRAW_RESULT, isBot, turns};
            break;
        }

        currentPlayer = 1 - currentPlayer;
    }
    return result;
}

void endGame(const RunConfig& config, GameSetup& gameSetup, GameResult& gameResult) {
    if (config.interactive) {
        clearScreen();
        std::cout << "\n\033[1;36m--- FINAL BOARD ---\033[0m\n";
        displayBoard(gameSetup.board, gameSetup.size, &gameSetup, gameResult.turns);
        std::cout << "\n";
        showResult(gameResult.winner, gameResult.isBot);
    } else {
        printResult(gameResult);
    }

    std::string winnerStr = (gameResult.winner == DRAW_RESULT)
        ? "Draw"
        : "Player " + std::to_string(gameResult.winner + 1) +
          (gameResult.isBot ? " (Bot)" : " (Human)");

    GameLogger::log("Game finished after " + std::to_string(gameResult.turns) +
                    " turns. Result: " + winnerStr, GameLogger::Level::INFO);
}
