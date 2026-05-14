/**
 * SDL Interaction header file
 */

#pragma once

#include <SDL2/SDL.h>

#include "../game/interface/i_interaction.h"
#include "../game/setup.h"
#include "../utils/config.h"

class SDLInteraction : public I_Interaction {
   private:
    bool waitForQuit(SDL_Event& e);

    // Board layout context (set by engine before getPlayerMove is called)
    int boardSize = 0;
    int originX   = 0;
    int originY   = 0;
    int cellSize  = 0;

   public:
    SDLInteraction();
    ~SDLInteraction();

    void init(const RunConfig& config) override;
    void pause(int timeout = 0) override;
    bool selectSize(int* size) override;
    bool selectGoal(int* goal, const int size) override;
    bool selectGameMode(GameMode* mode) override;
    bool selectBotLevel(BotLevel* levels, const int index) override;
    bool getPlayerMove(int* row, int* col) override;
    void close() override;

    // Called by engine to inform interaction about current board layout
    void setBoardContext(int sz, int originX, int originY, int cellSize);
};
