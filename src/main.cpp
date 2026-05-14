#include "utils/config.h"
#include "utils/logger.h"
#include "utils/helper.h"
#include "game/engine.h"
#include "terminal/interaction.h"
#include <iostream>
#include <memory>

// Forward declarations for factory functions
// Defined in engine.cpp to keep main.cpp clean
void runGame(const RunConfig& config);

int main(int argc, char* argv[]) {
#ifdef _WIN32
    std::system("mode con cols=120 lines=45");
#endif

    RunConfig config = parseArgs(argc, argv);

    GameLogger::init(config.judge_mode, config.to_file, config.log_file);
    GameLogger::log("GameLogger initialized!");
    GameLogger::log(std::string("GUI mode: ") + (config.gui_mode ? "ON" : "OFF"));

    runGame(config);

    GameLogger::log("GameLogger closing . . .");
    GameLogger::close();

    return 0;
}
