/**
 * I Renderer header file
 *
 */

#pragma once

/* ---------- Importing ---------- */

#include "../setup.h"
#include "../../utils/config.h"
#include "../logic.h"

/* ---------- Declarations ---------- */

/**
 * Mô tả:
 *   Interface I_Renderer định nghĩa các hàm cần thiết để hiển thị UI của game.
 *   Các implementation cụ thể (TerminalRenderer, SDLRenderer, ...) sẽ kế thừa class này.
 *
 *   Engine sẽ sử dụng interface này để:
 *   - Render board
 *   - Hiển thị menu chọn
 *   - Hiển thị kết quả
 *
 * Đầu vào:
 *   - Các dữ liệu game (board, player, result, ...)
 *
 * Đầu ra:
 *   - Hiển thị UI (console hoặc GUI)
 *
 * Tác dụng phụ:
 *   - In ra màn hình (terminal hoặc cửa sổ đồ họa)
 *
 * NOTE:
 *   - Đây là abstract class (có các hàm pure virtual)
 */
class I_Renderer {
   public:
    I_Renderer();
    virtual ~I_Renderer();

    virtual void init(const RunConfig& config) = 0;
    virtual void clearScreen() = 0;
    virtual void showSelectMenu(SelectType selectType, int context = NO_CONTEXT) = 0;
    virtual void showInvalidSelect(SelectType selectType, int context = NO_CONTEXT) = 0;
    virtual void showValidSelect(SelectType selectType, int context = NO_CONTEXT) = 0;
    virtual void displayBoard(const char board[][BOARD_N_MAX], const int size) = 0;
    virtual void showMove(const int row, const int col) = 0;
    virtual void showInvalidMove() = 0;
    virtual void showPlayer(const int player, const bool is_bot) = 0;
    virtual void showResult(const int winner, const bool is_bot, const WinLine* winLine = nullptr) = 0;
    virtual void printResult(const GameResult& gameResult) = 0;
    virtual void close() = 0;
};
