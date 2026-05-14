#include "helper.h"
#include <iostream>

RunConfig parseArgs(int argc, char* argv[]) {
    RunConfig config;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-j" || arg == "--judge") {
            config.judge_mode   = true;
            config.interactive  = false;
        } else if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            config.input_file = argv[++i];
        } else if ((arg == "-l" || arg == "--log") && i + 1 < argc) {
            config.log_file = argv[++i];
            if (config.log_file.empty()) config.to_file = false;
        } else if (arg == "-g" || arg == "--gui") {
            config.gui_mode = true;
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Tic-tac-toe Game (Version: " << VERSION << ")\n";
            std::cout << "Usage:\n"
                      << "\t--judge, -j\tEnable judge mode\n"
                      << "\t--input, -i\tPath to input file\n"
                      << "\t--log,   -l\tPath to log file (default: log.txt)\n"
                      << "\t--gui,   -g\tEnable SDL GUI mode\n";
            exit(0);
        }
    }
    return config;
}
