/**
 * Setup header file
 *
 * NOTE: Các enum/struct/constant chính đã được chuyển sang utils/config.h
 *       File này chỉ giữ lại những thứ đặc thù của game logic (exception, random, alias).
 */

#pragma once

/* ---------- Importing ---------- */

#include <exception>
#include <random>
#include <utility>

#include "../utils/config.h"

/* ---------- Không trùng với config.h ---------- */

// ---------- Constants (chỉ có trong setup.h) ----------

constexpr int BOARD_N_MIN  = 3;    // kích thước bàn cờ nhỏ nhất
constexpr int GOAL_MAX     = 5;    // số lượng quân cần để thắng tối đa

// ---------- Minimax / AI ----------

constexpr int SCORE_INF      = 1000;   // giá trị vô cùng (dùng trong minimax)
constexpr int EVALUATE_SCORE = 10;    // điểm đánh giá cơ bản

// ---------- Random ----------
// generator được khai báo trong config.h (extern) và định nghĩa trong config.cpp
// KHÔNG định nghĩa lại ở đây để tránh LNK2005

/* ---------- Type Definitions ---------- */

// alias cho pair<int, int> (ọa độ)
using pII = std::pair<int, int>;

/**
 * Mô tả:
 *   Exception dùng để signal việc người dùng muốn thoát game.
 */
class QuitException : public std::exception {
   public:
    const char* what() const noexcept override {
        return "User requested quit";
    }
};

/**
 * Mô tả: Exception dùng để báo hiệu một chức năng chưa được cài đặt.
 */
class NotImplementedException : public std::exception {
   public:
    const char* what() const noexcept override {
        return "Functionality not implemented yet";
    }
};
