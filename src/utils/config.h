#pragma once
#include <string>
#include <random>
#include <utility>

// ─── Version ────────────────────────────────────────────────
inline const std::string VERSION = "0.4.25022782";

// ─── Flags ──────────────────────────────────────────────────
constexpr bool ALGORITHM_FLAG = true;
constexpr bool TIME_ENABLED   = true;
constexpr int  RANDOM_SEED    = 2013;

// ─── Constants ──────────────────────────────────────────────
constexpr int BOARD_N_MAX  = 12;
constexpr int SLEEP_TIME   = 1500;
constexpr int NO_CONTEXT   = -1;
constexpr int DRAW_RESULT  = -1;

// ─── SDL Window defaults ────────────────────────────────────
constexpr int DEFAULT_SCREEN_W = 960;
constexpr int DEFAULT_SCREEN_H = 720;

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
typedef std::pair<int,int> pII;

// ─── Structs ────────────────────────────────────────────────
struct RunConfig {
    bool        interactive  = true;
    bool        judge_mode   = false;
    bool        gui_mode     = false;
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
    int  winner = DRAW_RESULT;
    bool isBot  = false;
    int  turns  = 0;
};
