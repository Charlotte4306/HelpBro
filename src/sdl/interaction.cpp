/**
 * SDL Interaction cpp implementation
 */

#include "interaction.h"

#include <format>
#include <iostream>
#include <string>

// ────────────────────────────────────────────
// Constructor / Destructor
// ────────────────────────────────────────────

SDLInteraction::SDLInteraction() {}
SDLInteraction::~SDLInteraction() {}

// ────────────────────────────────────────────
// Quit helper
// ────────────────────────────────────────────

bool SDLInteraction::waitForQuit(SDL_Event& e) {
    if (e.type == SDL_QUIT) throw QuitException();
    return false;
}

// ────────────────────────────────────────────
// init / close
// ────────────────────────────────────────────

void SDLInteraction::init(const RunConfig&) {
    // nothing to init — SDL is started by SDLRenderer
}

void SDLInteraction::close() {}

// ────────────────────────────────────────────
// pause
// ────────────────────────────────────────────

void SDLInteraction::pause(int timeout) {
    if (timeout > 0) {
        SDL_Delay(timeout);
        return;
    }
    bool waiting = true;
    SDL_Event e;
    while (waiting) {
        if (SDL_WaitEvent(&e)) {
            waitForQuit(e);
            if (e.type == SDL_KEYDOWN || e.type == SDL_MOUSEBUTTONDOWN)
                waiting = false;
        }
    }
}

// ────────────────────────────────────────────
// Internal: read a single digit key (1-9) from SDL events.
// Returns the digit, or -1 if user closed the window.
// ────────────────────────────────────────────

static int waitForDigit() {
    SDL_Event e;
    while (true) {
        if (SDL_WaitEvent(&e)) {
            if (e.type == SDL_QUIT) throw QuitException();
            if (e.type == SDL_KEYDOWN) {
                SDL_Keycode k = e.key.keysym.sym;
                if (k >= SDLK_1 && k <= SDLK_9)
                    return (int)(k - SDLK_0);
                if (k >= SDLK_KP_1 && k <= SDLK_KP_9)
                    return (int)(k - SDLK_KP_0);
            }
        }
    }
}

// ────────────────────────────────────────────
// selectSize
// ────────────────────────────────────────────

bool SDLInteraction::selectSize(int* size) {
    int v = waitForDigit();
    if (v >= 3 && v <= BOARD_N_MAX) {
        *size = v;
        return true;
    }
    return false;
}

// ────────────────────────────────────────────
// selectGoal
// ────────────────────────────────────────────

bool SDLInteraction::selectGoal(int* goal, const int size) {
    int v = waitForDigit();
    if (v >= 3 && v <= size) {
        *goal = v;
        return true;
    }
    return false;
}

// ────────────────────────────────────────────
// selectGameMode
// ────────────────────────────────────────────

bool SDLInteraction::selectGameMode(GameMode* mode) {
    int v = waitForDigit();
    if (v >= 1 && v <= 3) {
        *mode = static_cast<GameMode>(v - 1);
        return true;
    }
    return false;
}

// ────────────────────────────────────────────
// selectBotLevel
// ────────────────────────────────────────────

bool SDLInteraction::selectBotLevel(BotLevel* levels, const int index) {
    if (!levels || index < 0 || index > 1) return false;
    int v = waitForDigit();
    if (v >= 1 && v <= 3) {
        levels[index] = static_cast<BotLevel>(v - 1);
        return true;
    }
    return false;
}

// ────────────────────────────────────────────
// getPlayerMove — wait for mouse click on board
// We store board metrics via a shared context set by the engine.
// Simple approach: store last known board size in interaction.
// ────────────────────────────────────────────

void SDLInteraction::setBoardContext(int sz, int ox, int oy, int cell) {
    boardSize = sz;
    originX   = ox;
    originY   = oy;
    cellSize  = cell;
}

bool SDLInteraction::getPlayerMove(int* row, int* col) {
    SDL_Event e;
    while (true) {
        if (SDL_WaitEvent(&e)) {
            if (e.type == SDL_QUIT) throw QuitException();
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x;
                int my = e.button.y;
                if (cellSize <= 0) return false;
                int c = (mx - originX) / cellSize;
                int r = (my - originY) / cellSize;
                if (r >= 0 && r < boardSize && c >= 0 && c < boardSize) {
                    *row = r;
                    *col = c;
                    return true;
                }
                // click outside board — let caller retry
                return false;
            }
        }
    }
}
