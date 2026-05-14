#pragma once
#include "../utils/config.h"

// Public entry point: picks terminal or SDL based on config.gui_mode
void runGame(const RunConfig& config);

// Legacy signatures (kept for backward compatibility)
void startGame(const RunConfig& config, GameSetup& gameSetup);
GameResult playGame(const RunConfig& config, GameSetup& gameSetup);
void endGame(const RunConfig& config, GameSetup& gameSetup, GameResult& gameResult);
