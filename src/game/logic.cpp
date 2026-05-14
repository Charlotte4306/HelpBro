#include "logic.h"

void initBoard(char board[][BOARD_N_MAX], const int size) {
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            board[r][c] = '-';
}

bool isValidMove(const char board[][BOARD_N_MAX], const int size, const int row, const int col) {
    if (row < 0 || row >= size || col < 0 || col >= size) return false;
    return board[row][col] == '-';
}

void makeMove(char board[][BOARD_N_MAX], const int row, const int col, const char symbol) {
    board[row][col] = symbol;
}

bool isEmptyHead(char board[][BOARD_N_MAX], int size, int x, int y, const char symbol) {
    if (x < 0 || x >= size || y < 0 || y >= size) return true;
    return board[x][y] == '-' || board[x][y] == symbol;
}

bool checkWin(char board[][BOARD_N_MAX], const int size, const char symbol, const int goal, EndRule rule) {
    const int dx[4] = {0, 1, 1,  1};
    const int dy[4] = {1, 0, 1, -1};

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board[r][c] != symbol) continue;

            for (int dir = 0; dir < 4; ++dir) {
                int count = 0;
                for (int i = 0; i < goal; ++i) {
                    int nx = r + i * dx[dir];
                    int ny = c + i * dy[dir];
                    if (nx >= 0 && nx < size && ny >= 0 && ny < size && board[nx][ny] == symbol)
                        count++;
                    else break;
                }

                if (count == goal) {
                    int prevX = r - dx[dir], prevY = c - dy[dir];
                    int nextX = r + goal * dx[dir], nextY = c + goal * dy[dir];

                    bool end1 = isEmptyHead(board, size, prevX, prevY, symbol);
                    bool end2 = isEmptyHead(board, size, nextX, nextY, symbol);

                    if      (rule == EndRule::OPEN_TWO) { if (end1 && end2) return true; }
                    else if (rule == EndRule::OPEN_ONE) { if (end1 || end2) return true; }
                    else return true;
                }
            }
        }
    }
    return false;
}

bool checkDraw(char board[][BOARD_N_MAX], const int size) {
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (board[r][c] == '-') return false;
    return true;
}
