#include "engine.h"
#include "logic.h"
#include "bot/bot.h"
#include "interface/i_renderer.h"
#include "interface/i_interaction.h"
#include "../terminal/interaction.h"
#include "../terminal/renderer.h"
#include "../utils/logger.h"
#include "../utils/helper.h"
#include <format>
#include <thread>
#include <chrono>
#include <limits>
#include <iostream>
#include <memory>

// ──────────────────────────────────────────────────
// Terminal wrappers implementing I_Renderer / I_Interaction
// so engine.cpp can use them via the same interface.
// ──────────────────────────────────────────────────

class TerminalRenderer : public I_Renderer {
public:
    void init(const RunConfig&) override {}
    void clearScreen() override { ::clearScreen(); }
    void showSelectMenu(SelectType t, int ctx) override {
        (void)ctx;
        ::showSelectMenu(t);
    }
    void showInvalidSelect(SelectType, int) override {}
    void showValidSelect(SelectType, int) override {}
    void displayBoard(const char board[][BOARD_N_MAX], const int size) override {
        ::displayBoard(board, size);
    }
    void showMove(const int row, const int col) override { ::showMove(row, col); }
    void showInvalidMove() override { ::showInvalidMove(); }
    void showPlayer(const int player, const bool is_bot) override { ::showPlayer(player, is_bot); }
    void showResult(const int winner, const bool is_bot, const WinLine*) override {
        ::showResult(winner, is_bot);
    }
    void printResult(const GameResult& r) override { ::printResult(r); }
    void close() override {}
};

class TerminalInteractionWrapper : public I_Interaction {
public:
    std::streambuf* cin_backup = nullptr;
    void init(const RunConfig& config) override {
        cin_backup = initInteraction(config);
    }
    void pause(int timeout) override {
        if (timeout > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
    }
    bool selectSize(int* size) override { return ::selectSize(size); }
    bool selectGoal(int* goal, const int size) override { return ::selectGoal(goal, size); }
    bool selectGameMode(GameMode* mode) override { return ::selectGameMode(mode); }
    bool selectBotLevel(BotLevel* levels, const int index) override { return ::selectBotLevel(levels, index); }
    bool getPlayerMove(int* row, int* col) override { return ::getPlayerMove(row, col); }
    void close() override { closeInteraction(cin_backup); }
};

// ──────────────────────────────────────────────────
// Core engine logic (now uses I_Renderer / I_Interaction)
// ──────────────────────────────────────────────────

static void startGame(I_Renderer& rnd, I_Interaction& inp,
                      const RunConfig& config, GameSetup& gs) {
    if (config.interactive) rnd.clearScreen();
    if (config.interactive) rnd.showSelectMenu(SelectType::TITLE_UI);

    do {
        if (config.interactive) rnd.showSelectMenu(SelectType::SIZE_UI);
    } while (!inp.selectSize(&gs.size));

    do {
        if (config.interactive) {
            rnd.showSelectMenu(SelectType::GOAL_UI, gs.size);
        }
    } while (!inp.selectGoal(&gs.goal, gs.size));

    do {
        if (config.interactive) rnd.showSelectMenu(SelectType::GAME_MODE_UI);
    } while (!inp.selectGameMode(&gs.mode));

    if (gs.mode == GameMode::PVE) {
        do {
            if (config.interactive) rnd.showSelectMenu(SelectType::BOT_LEVEL_UI);
        } while (!inp.selectBotLevel(gs.levels, 1));
    }

    if (gs.mode == GameMode::EVE) {
        do {
            if (config.interactive) rnd.showSelectMenu(SelectType::MUL_BOT_LEVEL_UI);
        } while (!(inp.selectBotLevel(gs.levels, 0) && inp.selectBotLevel(gs.levels, 1)));
    }

    initBoard(gs.board, gs.size);
}

static GameResult playGame(I_Renderer& rnd, I_Interaction& inp,
                           const RunConfig& config, GameSetup& gs) {
    GameResult result;
    int currentPlayer = 0;
    char symbols[2] = {'X', 'O'};
    int turns = 0;

    while (true) {
        if (config.interactive) {
            rnd.clearScreen();
            rnd.displayBoard(gs.board, gs.size);
        }

        bool isBot = (gs.mode == GameMode::EVE) ||
                     (gs.mode == GameMode::PVE && currentPlayer == 1);

        if (config.interactive) rnd.showPlayer(currentPlayer + 1, isBot);

        int row = -1, col = -1;

        if (isBot) {
            std::string label = std::format("Bot {} ({})", currentPlayer + 1, symbols[currentPlayer]);
            pII point = measureExecutionTime(label, [&]() {
                return botMove(gs.board, gs.size, gs.goal,
                               symbols[currentPlayer], gs.levels[currentPlayer]);
            }, TIME_ENABLED);

            row = point.first;
            col = point.second;

            if (!isValidMove(gs.board, gs.size, row, col)) {
                GameLogger::log(std::format("WARNING: Bot {} invalid move ({},{}) - fallback to random.",
                                currentPlayer + 1, row, col), GameLogger::Level::WARNING);
                auto fb = random_pick(gs.board, gs.size);
                row = fb.first; col = fb.second;
            }

            if (config.interactive)
                inp.pause(SLEEP_TIME);
        } else {
            bool validMove = false;
            while (!validMove) {
                if (config.interactive) rnd.showSelectMenu(SelectType::PLAYER_UI);
                if (inp.getPlayerMove(&row, &col) && isValidMove(gs.board, gs.size, row, col)) {
                    validMove = true;
                } else {
                    if (config.interactive) rnd.showInvalidMove();
                    if (!config.gui_mode) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    }
                }
            }
        }

        makeMove(gs.board, row, col, symbols[currentPlayer]);
        turns++;

        if (symbols[currentPlayer] == 'X') gs.lastX = {row, col};
        else                               gs.lastO = {row, col};

        if (config.interactive) {
            rnd.showMove(row, col);
            inp.pause(2000);
        }

        if (checkWin(gs.board, gs.size, symbols[currentPlayer], gs.goal)) {
            result = {currentPlayer, isBot, turns};
            break;
        }
        if (checkDraw(gs.board, gs.size)) {
            result = {DRAW_RESULT, isBot, turns};
            break;
        }

        currentPlayer = 1 - currentPlayer;
    }
    return result;
}

static void endGame(I_Renderer& rnd, I_Interaction&,
                    const RunConfig& config, GameSetup& gs, GameResult& gr) {
    if (config.interactive) {
        rnd.clearScreen();
        rnd.displayBoard(gs.board, gs.size);
        rnd.showResult(gr.winner, gr.isBot);
    } else {
        rnd.printResult(gr);
    }

    std::string winnerStr = (gr.winner == DRAW_RESULT)
        ? "Draw"
        : "Player " + std::to_string(gr.winner + 1) +
          (gr.isBot ? " (Bot)" : " (Human)");

    GameLogger::log("Game finished after " + std::to_string(gr.turns) +
                    " turns. Result: " + winnerStr, GameLogger::Level::INFO);
}

// ──────────────────────────────────────────────────
// Public entry point called from main.cpp
// ──────────────────────────────────────────────────

void runGame(const RunConfig& config) {
    std::unique_ptr<I_Renderer>    rnd;
    std::unique_ptr<I_Interaction> inp;

    if (config.gui_mode) {
        // Lazy include to avoid pulling SDL2 headers when not needed
#include "../sdl/renderer.h"
#include "../sdl/interaction.h"
        rnd = std::make_unique<SDLRenderer>();
        inp = std::make_unique<SDLInteraction>();
    } else {
        rnd = std::make_unique<TerminalRenderer>();
        inp = std::make_unique<TerminalInteractionWrapper>();
    }

    rnd->init(config);
    GameLogger::log("Renderer initialized!");

    inp->init(config);
    GameLogger::log("Interaction initialized!");

    GameSetup gs;
    startGame(*rnd, *inp, config, gs);
    GameLogger::log("GameEngine startGame done!");

    GameResult gr = playGame(*rnd, *inp, config, gs);
    GameLogger::log("GameEngine playGame done!");

    endGame(*rnd, *inp, config, gs, gr);
    GameLogger::log("GameEngine endGame done!");

    inp->close();
    rnd->close();
}

// Legacy stubs — kept so old call sites compile if any remain.
void startGame(const RunConfig& config, GameSetup& gs) {
    TerminalInteractionWrapper inp;
    TerminalRenderer rnd;
    inp.init(config);
    startGame(rnd, inp, config, gs);
}
GameResult playGame(const RunConfig& config, GameSetup& gs) {
    TerminalInteractionWrapper inp;
    TerminalRenderer rnd;
    inp.init(config);
    return playGame(rnd, inp, config, gs);
}
void endGame(const RunConfig& config, GameSetup& gs, GameResult& gr) {
    TerminalRenderer rnd;
    TerminalInteractionWrapper inp;
    endGame(rnd, inp, config, gs, gr);
}
