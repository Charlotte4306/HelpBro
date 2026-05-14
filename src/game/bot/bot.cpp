#include "bot.h"
#include "../logic.h"
#include <random>
#include <cmath>
#include <algorithm>

// ─── Dispatcher ─────────────────────────────────────────────
pII botMove(char board[][BOARD_N_MAX], const int size, const int goal, const char symbol, const BotLevel level) {
    char opponent = (symbol == 'X') ? 'O' : 'X';
    switch (level) {
        case BotLevel::EASY:   return random_pick(board, size);
        case BotLevel::MEDIUM: return simple_heuristic(board, size, goal, symbol, opponent);
        case BotLevel::HARD:   return hard_level(board, size, goal, symbol, opponent);
        default:               return random_pick(board, size);
    }
}

// ─── EASY ────────────────────────────────────────────────────
pII random_pick(char board[][BOARD_N_MAX], const int size) {
    int emptyCount = 0;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (board[r][c] == '-') emptyCount++;

    if (emptyCount == 0) return {-1, -1};

    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, emptyCount - 1);
    int targetIndex = dist(gen);

    int currentIndex = 0;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (board[r][c] == '-') {
                if (currentIndex == targetIndex) return {r, c};
                currentIndex++;
            }
    return {-1, -1};
}

// ─── MEDIUM ──────────────────────────────────────────────────
pII simple_heuristic(char board[][BOARD_N_MAX], const int size, const int goal,
                     const char botSymbol, const char playerSymbol) {
    auto tryWin = [&](char sym) -> pII {
        for (int r = 0; r < size; ++r)
            for (int c = 0; c < size; ++c)
                if (board[r][c] == '-') {
                    board[r][c] = sym;
                    bool win = checkWin(board, size, sym, goal, EndRule::OPEN_TWO);
                    board[r][c] = '-';
                    if (win) return {r, c};
                }
        return {-1, -1};
    };

    pII move = tryWin(botSymbol);
    if (move.first != -1) return move;
    move = tryWin(playerSymbol);
    if (move.first != -1) return move;

    const int dx[4] = {0, 1, 1,  1};
    const int dy[4] = {1, 0, 1, -1};

    int pieceCount = 0;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (board[r][c] != '-') pieceCount++;

    static std::mt19937 gen(std::random_device{}());
    int noiseRange = (pieceCount < 6) ? 30 : 5;
    std::uniform_int_distribution<int> noise(0, noiseRange);

    int  bestScore = -10000;
    int  tieCount  = 0;
    pII  bestMove  = {-1, -1};

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board[r][c] != '-') continue;
            int score = 0;
            for (int dir = 0; dir < 4; ++dir) {
                int botCount = 0, playerCount = 0;
                for (int step : {-1, 1}) {
                    int nx = r + step * dx[dir], ny = c + step * dy[dir];
                    if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                        if      (board[nx][ny] == botSymbol)    botCount++;
                        else if (board[nx][ny] == playerSymbol) playerCount++;
                    }
                }
                score += (botCount * 15) + (playerCount * 10);
            }
            int centerDist = std::abs(r - size / 2) + std::abs(c - size / 2);
            int totalScore = score * 10 - (centerDist * 3) + noise(gen);

            if (totalScore > bestScore) {
                bestScore = totalScore;
                bestMove  = {r, c};
                tieCount  = 1;
            } else if (totalScore == bestScore) {
                tieCount++;
                if (std::uniform_int_distribution<int>(0, tieCount - 1)(gen) == 0)
                    bestMove = {r, c};
            }
        }
    }
    return (bestMove.first != -1) ? bestMove : random_pick(board, size);
}

// ─── HARD ────────────────────────────────────────────────────
pII hard_level(char board[][BOARD_N_MAX], const int size, const int goal,
               const char botSymbol, const char playerSymbol) {
    auto evaluatePattern = [&](int r, int c, char sym) -> int {
        int score = 0, open_threes = 0, open_fours = 0;
        const int dx[4] = {0, 1, 1,  1};
        const int dy[4] = {1, 0, 1, -1};

        for (int dir = 0; dir < 4; ++dir) {
            int consecutive = 1, blocks = 0;

            int x1 = r + dx[dir], y1 = c + dy[dir];
            while (x1 >= 0 && x1 < size && y1 >= 0 && y1 < size && board[x1][y1] == sym)
            { consecutive++; x1 += dx[dir]; y1 += dy[dir]; }
            if (x1 < 0 || x1 >= size || y1 < 0 || y1 >= size || board[x1][y1] != '-') blocks++;

            int x2 = r - dx[dir], y2 = c - dy[dir];
            while (x2 >= 0 && x2 < size && y2 >= 0 && y2 < size && board[x2][y2] == sym)
            { consecutive++; x2 -= dx[dir]; y2 -= dy[dir]; }
            if (x2 < 0 || x2 >= size || y2 < 0 || y2 >= size || board[x2][y2] != '-') blocks++;

            if (blocks == 2 && consecutive < goal) continue;

            if      (consecutive >= goal)        score += 1000000;
            else if (consecutive == goal - 1) {
                if (blocks == 0) { score += 50000; open_fours++; }
                else               score += 10000;
            } else if (consecutive == goal - 2) {
                if (blocks == 0) { score += 5000; open_threes++; }
                else               score += 500;
            } else if (consecutive == goal - 3) {
                score += (blocks == 0) ? 100 : 10;
            }
        }
        if (open_threes >= 2)                        score += 40000;
        if (open_fours >= 1 && open_threes >= 1)     score += 45000;
        if (open_fours >= 2)                         score += 90000;
        return score;
    };

    static std::mt19937 gen(std::random_device{}());
    int bestScore = -1, tieCount = 0;
    pII bestMove  = {-1, -1};

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board[r][c] != '-') continue;

            int attackScore  = evaluatePattern(r, c, botSymbol);
            int defenseScore = evaluatePattern(r, c, playerSymbol);

            if (attackScore >= 1000000) return {r, c};

            int totalScore;
            if (defenseScore >= 1000000) {
                totalScore = 2000000;
            } else {
                int centerDist  = std::abs(r - size / 2) + std::abs(c - size / 2);
                int centerBonus = (size - centerDist) * 2;
                totalScore = attackScore + (defenseScore * 2) + centerBonus;
            }

            if (totalScore > bestScore) {
                bestScore = totalScore;
                bestMove  = {r, c};
                tieCount  = 1;
            } else if (totalScore == bestScore) {
                tieCount++;
                if (std::uniform_int_distribution<int>(0, tieCount - 1)(gen) == 0)
                    bestMove = {r, c};
            }
        }
    }
    return (bestMove.first != -1) ? bestMove : random_pick(board, size);
}
