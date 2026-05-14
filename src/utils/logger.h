#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

extern const std::string VERSION;

namespace GameLogger {

enum class Level {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    MSG,
};

inline std::string levelToString(Level level) {
    switch (level) {
        case Level::DEBUG:   return "DEBUG";
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARN";
        case Level::ERROR:   return "ERROR";
        case Level::MSG:     return "";
        default:             return "UNKNOWN";
    }
}

inline static Level min_level = Level::DEBUG;

const std::string RESET  = "\033[0m";
const std::string RED    = "\033[31m";
const std::string GREEN  = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE   = "\033[34m";
const std::string CYAN   = "\033[36m";

inline std::string getColor(Level level) {
    switch (level) {
        case Level::DEBUG:   return BLUE;
        case Level::INFO:    return GREEN;
        case Level::WARNING: return YELLOW;
        case Level::ERROR:   return RED;
        default:             return RESET;
    }
}

inline static std::ofstream log_file;
inline static bool write_to_file   = false;
inline static bool is_judge_mode   = false;

void init(bool judge_mode, bool to_file = true, const std::string& path = "log.txt");
void log(const std::string& msg, Level level = Level::INFO);
void close();

} // namespace GameLogger
