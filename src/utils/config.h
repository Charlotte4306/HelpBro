#pragma once
#include <string>
#include <random>

// ─── Version ────────────────────────────────────────────────
const std::string VERSION = "0.4.25022782";

// ─── Flags ──────────────────────────────────────────────────
const bool ALGORITHM_FLAG = true;
const bool TIME_ENABLED   = true;
const int  RANDOM_SEED    = 2013;

// ─── Constants ──────────────────────────────────────────────
const int BOARD_N_MAX = 12;
const int SLEEP_TIME  = 1500;

// ─── SDL Window defaults ────────────────────────────────────
const int DEFAULT_SCREEN_W = 960;
const int DEFAULT_SCREEN_H = 720;

// ─── Global RNG ─────────────────────────────────────────────
extern std::mt19937 generator;

// ─── Enums ──────────────────────────────────────────────────
enum class BotLevel   { EASY, MEDIUM, HARD, INVALID_LV };
enum class GameMode   { PVP, PVE, EVE, INVALID_MODE };
enum class SelectType {
    TITLE_UI, SIZE_UI, GOAL_UI, GAME_MODE_UI,
    BOT_LEVEL_UI, PLAYER_UI, MUL_BOT_LEVEL_UI, INVALID_UI
};
enum class EndRule    { NONE, OPEN_ONE, OPEN_TWO };

// ─── Type alias ─────────────────────────────────────────────
#include <utility>
typedef std::pair<int,int> pII;

// ─── Structs ────────────────────────────────────────────────
struct RunConfig {
    bool        interactive  = true;
    bool        judge_mode   = false;
    bool        gui_mode     = false;   // -g / --gui
    int         screenWidth  = DEFAULT_SCREEN_W;
    int         screenHeight = DEFAULT_SCREEN_H;
    std::string input_file;
    bool        to_file      = true;
    std::string log_file     = "log.txt";
};

struct GameSetup {
    char     board[BOARD_N_MAX][BOARD_N_MAX];
    int      size;
    int      goal;
    GameMode mode;
    BotLevel levels[2];
    pII      lastX = {-1, -1};
    pII      lastO = {-1, -1};
};

struct GameResult {
    int  winner;
    bool isBot;
    int  turns;
};

constexpr int DRAW_RESULT = -1;
