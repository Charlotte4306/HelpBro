# 📖 Hướng dẫn & Giải thích Code

## Mục lục
1. [Tổng quan kiến trúc](#1-tổng-quan-kiến-trúc)
2. [Luồng chạy chương trình](#2-luồng-chạy-chương-trình)
3. [Module utils](#3-module-utils)
4. [Module game/logic](#4-module-gamelogic)
5. [Module game/engine](#5-module-gameengine)
6. [Module bot](#6-module-bot)
7. [Module interface (Abstract Layer)](#7-module-interface-abstract-layer)
8. [Module terminal](#8-module-terminal)
9. [Module sdl](#9-module-sdl)
10. [Cách thêm bot mới](#10-cách-thêm-bot-mới)
11. [Cách thêm chế độ render mới](#11-cách-thêm-chế-độ-render-mới)

---

## 1. Tổng quan kiến trúc

Project dùng kiến trúc **Strategy Pattern** + **Abstract Interface** để tách biệt:
- Logic game (không phụ thuộc vào UI)
- Giao diện hiển thị (Terminal / SDL2 / tương lai: web...)
- Nhập liệu (bàn phím / chuột / file)

```
main.cpp
   └── runGame(config)
         ├── I_Renderer  ←── TerminalRenderer hoặc SDLRenderer
         ├── I_Interaction ←── TerminalInteraction hoặc SDLInteraction
         └── GameEngine
               ├── startGame()   — chọn cài đặt
               ├── playGame()    — vòng lặp chính
               └── endGame()     — hiển thị kết quả
```

---

## 2. Luồng chạy chương trình

### Bước 1 — `main.cpp`
```cpp
RunConfig config = parseArgs(argc, argv);
runGame(config);
```
`parseArgs` đọc các flag từ command line và điền vào `RunConfig`.

### Bước 2 — `engine.cpp: runGame()`
```cpp
// Chọn renderer và interaction tùy config.gui_mode
if (config.gui_mode) {
    rnd = std::make_unique<SDLRenderer>();
    inp = std::make_unique<SDLInteraction>();
} else {
    rnd = std::make_unique<TerminalRenderer>();
    inp = std::make_unique<TerminalInteractionWrapper>();
}
// Chạy 3 giai đoạn
startGame(rnd, inp, config, gs);   // cài đặt game
playGame(rnd, inp, config, gs);    // vòng lặp đánh
endGame(rnd, inp, config, gs, gr); // kết quả
```

### Bước 3 — `startGame()`
Hỏi người chơi lần lượt:
- Kích thước bàn cờ (3–12)
- Goal — số quân liên tiếp để thắng
- Chế độ chơi (PVP / PVE / EVE)
- Mức độ bot (nếu cần)

### Bước 4 — `playGame()`
Vòng lặp `while(true)`:
1. Vẽ bàn cờ
2. Xác định lượt ai (người hay bot)
3. Nếu bot → gọi `botMove()`, nếu người → đọc input
4. `makeMove()` — cập nhật bàn cờ
5. `checkWin()` hoặc `checkDraw()` — kiểm tra kết thúc
6. Đổi lượt

---

## 3. Module utils

### `config.h`
File header duy nhất chứa **tất cả type toàn cục**. Mọi file đều include file này.

```cpp
// Hằng số — dùng constexpr để tránh LNK2005
constexpr int BOARD_N_MAX = 12;  // bàn cờ tối đa 12×12
constexpr int NO_CONTEXT  = -1;  // sentinel cho default argument
constexpr int DRAW_RESULT = -1;  // mã hòa

// Enum
enum class BotLevel  { EASY, MEDIUM, HARD, INVALID_LV };
enum class GameMode  { PVP, PVE, EVE, INVALID_MODE };
enum class SelectType { TITLE_UI, SIZE_UI, ... };

// Struct cấu hình
struct RunConfig { bool gui_mode; bool interactive; std::string input_file; ... };
struct GameSetup { char board[12][12]; int size; int goal; GameMode mode; ... };
struct GameResult { int winner; bool isBot; int turns; };

// RNG toàn cục — chỉ extern ở đây, định nghĩa trong config.cpp
extern std::mt19937 generator;
```

> ⚠️ **Quy tắc quan trọng:** Dùng `constexpr` cho int/bool. Dùng `inline const` cho `std::string`. KHÔNG dùng `const` thông thường cho biến toàn cục ở header → gây LNK2005.

### `helper.h/.cpp`
```cpp
RunConfig parseArgs(int argc, char* argv[]);
// Đọc: --gui, --no-interactive, --input <file>, --log <file>

template<typename F>
auto measureExecutionTime(const std::string& label, F func, bool enabled);
// Đo thời gian bot suy nghĩ, log ra màn hình nếu TIME_ENABLED = true
```

### `logger.h/.cpp`
```cpp
GameLogger::init(judge_mode, to_file, log_file);
GameLogger::log("message");                          // mặc định INFO
GameLogger::log("message", GameLogger::Level::WARNING);
GameLogger::close();
```
Log được ghi vào `log.txt` và/hoặc `stdout` tùy cấu hình.

---

## 4. Module game/logic

### `logic.h/.cpp` — Luật chơi thuần túy

```cpp
void initBoard(char board[][BOARD_N_MAX], int size);
// Điền '-' vào toàn bộ bàn cờ

bool isValidMove(const char board[][], int size, int row, int col);
// true nếu ô (row,col) trong bàn và còn trống

void makeMove(char board[][], int row, int col, char symbol);
// Đặt 'X' hoặc 'O' vào ô (row,col)

bool checkWin(char board[][], int size, char symbol, int goal, EndRule rule);
// Kiểm tra symbol có goal quân liên tiếp không
// EndRule::OPEN_TWO = phải có 2 đầu trống (luật caro)
// EndRule::NONE     = chỉ cần goal quân liên tiếp

bool checkDraw(char board[][], int size);
// true nếu bàn cờ đầy
```

**`struct WinLine`** — lưu tọa độ đường thắng để SDL2 có thể highlight:
```cpp
struct WinLine { int startRow, startCol, endRow, endCol; };
```

---

## 5. Module game/engine

`engine.cpp` là trung tâm điều phối, **không biết gì về** Terminal hay SDL2 — chỉ gọi qua interface.

### TerminalRenderer & TerminalInteractionWrapper
Đây là 2 class **wrapper nội bộ** (private trong engine.cpp) để bọc các free function của module terminal:
```cpp
class TerminalRenderer : public I_Renderer {
    void displayBoard(...) override { ::displayBoard(board, size); } // gọi free function
    ...
};
```
Thiết kế này giúp engine.cpp không phụ thuộc vào terminal cụ thể.

### Legacy stubs
Cuối file có 3 hàm cũ `startGame/playGame/endGame(config, gs)` — giữ lại để tương thích với code cũ, không nên dùng trong code mới.

---

## 6. Module bot

### Chuỗi kế thừa
```
Bot (abstract, bot_base.h)
 └── BotLevel1 (EASY)    — random_pick
      └── BotLevel2 (MEDIUM) — simple_heuristic
           └── BotLevel3 (HARD)   — pattern scoring
```
Mỗi level cao hơn có thể fallback về level thấp hơn vì kế thừa.

### `bot.h/.cpp` — Free functions (dispatcher)
```cpp
pII botMove(board, size, goal, symbol, level);
// Switch theo level → gọi random_pick / simple_heuristic / hard_level

pII random_pick(board, size);
// Chọn ngẫu nhiên 1 ô trống

pII simple_heuristic(board, size, goal, botSym, playerSym);
// 1. Nếu bot có thể thắng ngay → đánh
// 2. Nếu đối thủ sắp thắng → chặn
// 3. Score các ô dựa vào quân lân cận + khoảng cách tâm

pII hard_level(board, size, goal, botSym, playerSym);
// Pattern scoring:
// - Đếm quân liên tiếp theo 4 hướng (ngang, dọc, chéo)
// - Score theo: open_four > open_three > blocked_three > ...
// - Ưu tiên tấn công, sau đó phòng thủ
```

### `bot_factory.h/.cpp`
```cpp
Bot* BotFactory::createBot(BotLevel level, const char& symbol);
// TODO: chưa implement — cần tạo new BotLevel1/2/3 tùy theo level
```
> ⚠️ `BotFactory::createBot` hiện throw `NotImplementedException`. Nếu dùng OOP bot, cần implement phần này.

---

## 7. Module interface (Abstract Layer)

### `I_Renderer`
```cpp
class I_Renderer {
    virtual void init(const RunConfig&) = 0;
    virtual void clearScreen() = 0;
    virtual void showSelectMenu(SelectType, int context = NO_CONTEXT) = 0;
    virtual void displayBoard(const char board[][BOARD_N_MAX], int size) = 0;
    virtual void showResult(int winner, bool isBot, const WinLine* = nullptr) = 0;
    virtual void printResult(const GameResult&) = 0;
    // ...
};
```
Mọi renderer (Terminal, SDL2) đều implement interface này. Engine chỉ gọi qua con trỏ `I_Renderer*`.

### `I_Interaction`
```cpp
class I_Interaction {
    virtual bool selectSize(int* size) = 0;
    virtual bool selectGameMode(GameMode* mode) = 0;
    virtual bool selectBotLevel(BotLevel* levels, int index) = 0;
    virtual bool getPlayerMove(int* row, int* col) = 0;
    virtual void pause(int ms) = 0;
    // ...
};
```

---

## 8. Module terminal

### `terminal/renderer.cpp`
- In bàn cờ bằng ký tự ASCII
- Hiển thị menu bằng `std::cout`
- `showResult()` in kết quả thắng/thua/hòa

### `terminal/interaction.cpp`
- Đọc input bằng `std::cin` hoặc redirect từ file
- `initInteraction()`: nếu `!config.interactive && !input_file.empty()` → redirect `cin` từ file
- `closeInteraction()`: restore lại `cin` gốc sau khi xong
- Validate input: chỉ chấp nhận chuỗi số nguyên dương

---

## 9. Module sdl

### `sdl/renderer.cpp`
- Dùng `SDL_Renderer` để vẽ bàn cờ, quân cờ, menu
- `SDL_ttf` để render chữ
- `showResult()` nhận thêm `WinLine*` để highlight đường thắng

### `sdl/interaction.cpp`
- Xử lý sự kiện `SDL_Event`
- Click chuột → convert tọa độ pixel → (row, col)
- `pause(ms)` → `SDL_Delay(ms)` thay vì `sleep_for`

---

## 10. Cách thêm bot mới

1. Tạo `src/game/bot/bot_lv4.h` và `bot_lv4.cpp`:
```cpp
// bot_lv4.h
#pragma once
#include "bot_lv3.h"

class BotLevel4 : public BotLevel3 {
public:
    BotLevel4(const BotLevel& _level, const char& _symbol);
    ~BotLevel4() override;
    pII getMove(char board[][BOARD_N_MAX], int size, int goal) override;
};
```

2. Thêm `EXPERT` vào `enum class BotLevel` trong `config.h`

3. Thêm case vào `botMove()` trong `bot.cpp`:
```cpp
case BotLevel::EXPERT: return expert_level(board, size, goal, symbol, opponent);
```

4. Thêm case vào `BotFactory::createBot()` trong `bot_factory.cpp`

5. Cập nhật `selectBotLevel()` trong `terminal/interaction.cpp` để chấp nhận giá trị 4

---

## 11. Cách thêm chế độ render mới

1. Tạo class kế thừa `I_Renderer` và `I_Interaction`
2. Implement tất cả pure virtual function
3. Trong `engine.cpp::runGame()`, thêm điều kiện:
```cpp
if (config.web_mode) {
    rnd = std::make_unique<WebRenderer>();
    inp = std::make_unique<WebInteraction>();
}
```
4. Thêm flag `web_mode` vào `RunConfig` và `parseArgs()`

Engine không cần thay đổi gì — đây chính là lợi ích của Abstract Interface.
