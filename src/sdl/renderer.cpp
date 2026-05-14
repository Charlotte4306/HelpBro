/**
 * SDL Renderer cpp implementation
 */

#include "renderer.h"

#include <format>
#include <iostream>
#include <string>

#include "../game/setup.h"
#include "../utils/config.h"

// ────────────────────────────────────────────
// Layout helpers
// ────────────────────────────────────────────
static const int PANEL_W    = 300;  // right-side info panel width
static const int BOARD_PAD  = 40;   // padding around the board

static SDL_Color COL_BG      = {20,  20,  20,  255};
static SDL_Color COL_GRID    = {80,  80,  80,  255};
static SDL_Color COL_X       = {220, 80,  80,  255};
static SDL_Color COL_O       = {80,  180, 220, 255};
static SDL_Color COL_LAST    = {255, 220, 0,   80};
static SDL_Color COL_PANEL   = {30,  30,  40,  255};
static SDL_Color COL_TEXT    = {230, 230, 230, 255};
static SDL_Color COL_ERR     = {255, 80,  80,  255};
static SDL_Color COL_OK      = {80,  220, 120, 255};

// ────────────────────────────────────────────
// Constructor / Destructor
// ────────────────────────────────────────────

SDLRenderer::SDLRenderer() : I_Renderer() {}
SDLRenderer::~SDLRenderer() {}

// ────────────────────────────────────────────
// Private helpers
// ────────────────────────────────────────────

void SDLRenderer::renderPresent() {
    SDL_RenderPresent(renderer);
}

void SDLRenderer::drawRect(int x, int y, int w, int h, SDL_Color color, bool filled) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (filled)
        SDL_RenderFillRect(renderer, &rect);
    else
        SDL_RenderDrawRect(renderer, &rect);
}

// Draw a simple 'X' inside the given cell rect
static void drawX(SDL_Renderer* rnd, int x, int y, int cell) {
    SDL_SetRenderDrawColor(rnd, 220, 80, 80, 255);
    int m = cell / 5;
    SDL_RenderDrawLine(rnd, x + m, y + m, x + cell - m, y + cell - m);
    SDL_RenderDrawLine(rnd, x + m, y + cell - m, x + cell - m, y + m);
    // thicker lines
    SDL_RenderDrawLine(rnd, x + m + 1, y + m, x + cell - m + 1, y + cell - m);
    SDL_RenderDrawLine(rnd, x + m, y + m + 1, x + cell - m, y + cell - m + 1);
    SDL_RenderDrawLine(rnd, x + m + 1, y + cell - m, x + cell - m + 1, y + m);
    SDL_RenderDrawLine(rnd, x + m, y + cell - m + 1, x + cell - m, y + m + 1);
}

// Draw a simple 'O' (hollow circle via SDL_RenderDrawPoint)
static void drawO(SDL_Renderer* rnd, int cx, int cy, int r) {
    SDL_SetRenderDrawColor(rnd, 80, 180, 220, 255);
    for (int dx = -r; dx <= r; ++dx) {
        int dy = (int)SDL_sqrt(r * r - dx * dx);
        // 4 points per pair to thicken
        for (int t = -2; t <= 2; ++t) {
            SDL_RenderDrawPoint(rnd, cx + dx, cy + dy + t);
            SDL_RenderDrawPoint(rnd, cx + dx, cy - dy + t);
        }
    }
}

// Draw a text string using SDL_ttf (or a fallback rectangle if font unavailable)
void SDLRenderer::drawText(const std::string& text, int x, int y, SDL_Color color, int size) {
    if (!font) return;
    // Open a one-shot font at the requested size
    TTF_Font* f = TTF_OpenFont(fontPath.c_str(), size);
    if (!f) f = font;
    SDL_Color c = color;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text.c_str(), c);
    if (!surf) { if (f != font) TTF_CloseFont(f); return; }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
    if (f != font) TTF_CloseFont(f);
}

// Compute board area
void SDLRenderer::getBoardMetrics(int size, int& originX, int& originY, int& cellSize) const {
    int boardW = screenW - PANEL_W - BOARD_PAD * 2;
    int boardH = screenH - BOARD_PAD * 2;
    cellSize  = std::min(boardW, boardH) / size;
    originX   = BOARD_PAD + (boardW - cellSize * size) / 2;
    originY   = BOARD_PAD + (boardH - cellSize * size) / 2;
}

// ────────────────────────────────────────────
// init / close
// ────────────────────────────────────────────

void SDLRenderer::init(const RunConfig& config) {
    screenW = config.screenWidth;
    screenH = config.screenHeight;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    window = SDL_CreateWindow(
        "TicTacToe SDL",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenW, screenH, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Try to load a font from assets/
    fontPath = "assets/font.ttf";
    font = TTF_OpenFont(fontPath.c_str(), 18);
    // If no font found, font stays nullptr and text rendering is skipped
}

void SDLRenderer::close() {
    if (font) { TTF_CloseFont(font); font = nullptr; }
    TTF_Quit();
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);
    SDL_Quit();
}

// ────────────────────────────────────────────
// clearScreen
// ────────────────────────────────────────────

void SDLRenderer::clearScreen() {
    SDL_SetRenderDrawColor(renderer, COL_BG.r, COL_BG.g, COL_BG.b, 255);
    SDL_RenderClear(renderer);
    // draw right panel background
    drawRect(screenW - PANEL_W, 0, PANEL_W, screenH, COL_PANEL, true);
}

// ────────────────────────────────────────────
// showSelectMenu
// ────────────────────────────────────────────

void SDLRenderer::showSelectMenu(SelectType selectType, int context) {
    clearScreen();

    int x = (screenW - PANEL_W) / 2;
    int y = screenH / 3;

    std::string title, prompt;
    switch (selectType) {
    case SelectType::TITLE_UI:
        drawText("=== TicTacToe SDL ===", x - 120, y,      COL_OK,   28);
        drawText("Press any key to start", x - 110, y + 50, COL_TEXT, 18);
        break;
    case SelectType::SIZE_UI:
        drawText("Enter board size N",           x - 100, y,       COL_TEXT, 22);
        drawText("3 <= N <= 12",                 x -  70, y + 35,  COL_TEXT, 16);
        drawText("Keys: 3-9  or  type + Enter",  x - 120, y + 65,  COL_TEXT, 15);
        if (context > 0) {
            std::string s = "Current: " + std::to_string(context);
            drawText(s, x - 50, y + 100, COL_OK, 20);
        }
        break;
    case SelectType::GOAL_UI:
        drawText("Enter win goal",               x - 80,  y,       COL_TEXT, 22);
        if (context > 0) {
            std::string s = "3 <= goal <= " + std::to_string(context);
            drawText(s, x - 80, y + 35,  COL_TEXT, 16);
        }
        drawText("Keys: 3-9  or  type + Enter",  x - 120, y + 65,  COL_TEXT, 15);
        break;
    case SelectType::GAME_MODE_UI:
        drawText("Select Game Mode",   x - 90,  y,       COL_TEXT, 22);
        drawText("[1]  PvP",           x - 40,  y + 40,  COL_TEXT, 18);
        drawText("[2]  PvE",           x - 40,  y + 70,  COL_TEXT, 18);
        drawText("[3]  EvE",           x - 40,  y + 100, COL_TEXT, 18);
        break;
    case SelectType::BOT_LEVEL_UI:
        drawText("Select Bot Level",   x - 90,  y,       COL_TEXT, 22);
        drawText("[1]  Easy",          x - 40,  y + 40,  COL_TEXT, 18);
        drawText("[2]  Medium",        x - 50,  y + 70,  COL_TEXT, 18);
        drawText("[3]  Hard",          x - 40,  y + 100, COL_TEXT, 18);
        break;
    case SelectType::MUL_BOT_LEVEL_UI:
        drawText("Select Level for Bot 1", x - 110, y,      COL_TEXT, 20);
        drawText("[1] Easy  [2] Medium  [3] Hard", x - 150, y + 35, COL_TEXT, 16);
        drawText("Then select Level for Bot 2",    x - 130, y + 70, COL_TEXT, 16);
        break;
    case SelectType::PLAYER_UI:
        drawText("Your turn! Click a cell", x - 110, y, COL_OK, 20);
        break;
    default:
        break;
    }

    renderPresent();
}

// ────────────────────────────────────────────
// showInvalidSelect / showValidSelect
// ────────────────────────────────────────────

void SDLRenderer::showInvalidSelect(SelectType, int) {
    drawText("Invalid input — try again", 20, screenH - 40, COL_ERR, 16);
    renderPresent();
    SDL_Delay(800);
}

void SDLRenderer::showValidSelect(SelectType, int) {
    drawText("OK!", 20, screenH - 40, COL_OK, 16);
    renderPresent();
    SDL_Delay(400);
}

// ────────────────────────────────────────────
// displayBoard
// ────────────────────────────────────────────

void SDLRenderer::displayBoard(const char board[][BOARD_N_MAX], const int size) {
    clearScreen();

    int ox, oy, cell;
    getBoardMetrics(size, ox, oy, cell);

    // Draw grid lines
    for (int i = 0; i <= size; ++i) {
        SDL_SetRenderDrawColor(renderer, COL_GRID.r, COL_GRID.g, COL_GRID.b, 255);
        // horizontal
        SDL_RenderDrawLine(renderer, ox, oy + i * cell, ox + size * cell, oy + i * cell);
        // vertical
        SDL_RenderDrawLine(renderer, ox + i * cell, oy, ox + i * cell, oy + size * cell);
    }

    // Draw pieces
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            int cx = ox + c * cell;
            int cy = oy + r * cell;
            if (board[r][c] == 'X') {
                drawX(renderer, cx + 2, cy + 2, cell - 4);
            } else if (board[r][c] == 'O') {
                drawO(renderer, cx + cell / 2, cy + cell / 2, cell / 2 - 6);
            }
        }
    }

    // Panel: coordinates hint
    drawText("Board", screenW - PANEL_W + 10, 10, COL_TEXT, 18);

    renderPresent();
}

// ────────────────────────────────────────────
// showMove
// ────────────────────────────────────────────

void SDLRenderer::showMove(const int row, const int col) {
    std::string msg = "Move: (" + std::to_string(row) + ", " + std::to_string(col) + ")";
    drawText(msg, screenW - PANEL_W + 10, 40, COL_OK, 16);
    renderPresent();
}

// ────────────────────────────────────────────
// showInvalidMove
// ────────────────────────────────────────────

void SDLRenderer::showInvalidMove() {
    drawText("Invalid move! Try again.", screenW - PANEL_W + 10, 70, COL_ERR, 16);
    renderPresent();
    SDL_Delay(600);
}

// ────────────────────────────────────────────
// showPlayer
// ────────────────────────────────────────────

void SDLRenderer::showPlayer(const int player, const bool is_bot) {
    std::string msg = "Player " + std::to_string(player);
    msg += is_bot ? " (Bot)" : " (Human)";
    drawText(msg, screenW - PANEL_W + 10, 100, COL_TEXT, 18);
    renderPresent();
}

// ────────────────────────────────────────────
// showResult
// ────────────────────────────────────────────

void SDLRenderer::showResult(const int winner, const bool is_bot, const WinLine*) {
    clearScreen();
    int x = (screenW - PANEL_W) / 2 - 100;
    int y = screenH / 2 - 30;

    if (winner == DRAW_RESULT) {
        drawText("=== DRAW! ===", x, y, COL_TEXT, 30);
    } else {
        std::string msg = "Player " + std::to_string(winner + 1);
        msg += is_bot ? " (Bot) WINS!" : " WINS!";
        drawText(msg, x, y, COL_OK, 30);
    }
    drawText("Press any key to exit.", x, y + 60, COL_TEXT, 16);
    renderPresent();
}

// ────────────────────────────────────────────
// printResult  (judge mode stdout)
// ────────────────────────────────────────────

void SDLRenderer::printResult(const GameResult& r) {
    if (r.winner == DRAW_RESULT) {
        std::cout << "Draw\n";
    } else {
        std::cout << "Player " << (r.winner + 1)
                  << (r.isBot ? " Bot" : " Human") << "\n";
    }
}
