/**
 * SDL Renderer header file
 */

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "../game/interface/i_renderer.h"
#include "../game/logic.h"

class SDLRenderer : public I_Renderer {
   private:
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font*     font     = nullptr;
    std::string   fontPath;
    int screenW = 960;
    int screenH = 720;

    void renderPresent();
    void drawRect(int x, int y, int w, int h, SDL_Color color, bool filled);
    void drawText(const std::string& text, int x, int y, SDL_Color color, int size = 18);
    void getBoardMetrics(int size, int& originX, int& originY, int& cellSize) const;

   public:
    SDLRenderer();
    ~SDLRenderer();

    void init(const RunConfig& config) override;
    void clearScreen() override;
    void showSelectMenu(SelectType selectType, int context = NO_CONTEXT) override;
    void showInvalidSelect(SelectType selectType, int context = NO_CONTEXT) override;
    void showValidSelect(SelectType selectType, int context = NO_CONTEXT) override;
    void displayBoard(const char board[][BOARD_N_MAX], const int size) override;
    void showMove(const int row, const int col) override;
    void showInvalidMove() override;
    void showPlayer(const int player, const bool is_bot) override;
    void showResult(const int winner, const bool is_bot, const WinLine* winLine = nullptr) override;
    void printResult(const GameResult& gameResult) override;
    void close() override;
};
