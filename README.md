# 🎮 TicTacToe LV2

Game Cờ Caro (Tic-Tac-Toe mở rộng) viết bằng **C++20**, hỗ trợ chế độ terminal và GUI (SDL2).  
Project môn học — nhóm phát triển tại trường Đại học.

---

## ✨ Tính năng

- **3 chế độ chơi:** PVP (người vs người), PVE (người vs bot), EVE (bot vs bot)
- **3 mức độ bot:** EASY (random), MEDIUM (heuristic), HARD (pattern scoring)
- **Bàn cờ linh hoạt:** kích thước từ 3×3 đến 12×12
- **2 giao diện:** Terminal (console) và GUI (SDL2 + SDL2_ttf)
- **Chế độ judge:** chạy tự động từ file input, hỗ trợ auto-grading
- **Logger:** ghi log ra file để debug và chấm điểm

---

## 🗂️ Cấu trúc thư mục

```
TicTacToe-LV2/
├── CMakeLists.txt          # Build configuration
├── grader.py               # Script chấm điểm tự động
├── assets/                 # Font, ảnh dùng cho SDL2 GUI
└── src/
    ├── main.cpp             # Entry point
    ├── utils/
    │   ├── config.h/.cpp    # Hằng số, enum, struct toàn cục
    │   ├── helper.h/.cpp    # Parse args, tiện ích
    │   └── logger.h/.cpp    # Hệ thống ghi log
    ├── game/
    │   ├── setup.h          # Exception, alias pII
    │   ├── logic.h/.cpp     # Luật chơi: initBoard, checkWin, checkDraw
    │   ├── engine.h/.cpp    # Game loop chính
    │   ├── interface/
    │   │   ├── i_renderer.h/.cpp    # Interface hiển thị (abstract)
    │   │   └── i_interaction.h/.cpp # Interface nhập liệu (abstract)
    │   └── bot/
    │       ├── bot_base.h   # Abstract class Bot
    │       ├── bot.h/.cpp   # Free functions: botMove, random_pick, ...
    │       ├── bot_lv1.*    # Bot EASY  — random
    │       ├── bot_lv2.*    # Bot MEDIUM — heuristic
    │       ├── bot_lv3.*    # Bot HARD  — pattern scoring
    │       └── bot_factory.*# Factory tạo bot theo level
    ├── terminal/
    │   ├── renderer.*       # Hiển thị terminal
    │   └── interaction.*    # Nhập liệu terminal / file
    └── sdl/
        ├── renderer.*       # Render SDL2 GUI
        └── interaction.*    # Nhập liệu chuột/bàn phím SDL2
```

---

## 🔧 Yêu cầu

| Thành phần | Phiên bản |
|---|---|
| C++ | 20 trở lên |
| CMake | 3.10 trở lên |
| SDL2 | 2.30.x |
| SDL2_ttf | 2.24.x |
| Compiler | MSVC 2022 / GCC 13+ / Clang 16+ |

**Cài SDL2 trên Windows:** Giải nén vào `C:\SDL2\`, cấu trúc:
```
C:\SDL2\SDL2-2.30.11\
C:\SDL2\SDL2_ttf-2.24.0\
C:\SDL2\DLL\SDL2.dll
C:\SDL2\DLL\SDL2_ttf.dll
```

---

## 🚀 Build & Chạy

### Visual Studio 2022
1. `File → Open → Folder...` → chọn thư mục project
2. CMake tự nhận diện `CMakeLists.txt`
3. Chọn configuration **x64-Debug** hoặc **x64-Release**
4. `Build → Build All` (hoặc `Ctrl+Shift+B`)

### Command line (MinGW / MSVC)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Chạy game
```bash
# Chế độ terminal (interactive)
game.exe

# Chế độ GUI (SDL2)
game.exe --gui

# Chế độ judge (đọc từ file)
game.exe --no-interactive --input input.txt

# Chạy với log file tùy chọn
game.exe --log mylog.txt
```

---

## 📝 Format Input (Judge Mode)

Khi dùng `--no-interactive --input <file>`, chương trình đọc lần lượt:

```
<board_size>          # 3..12
<goal>                # 3..size
<game_mode>           # 1=PVP, 2=PVE, 3=EVE
<bot_level>           # 1=EASY, 2=MEDIUM, 3=HARD  (nếu mode=PVE)
<bot1_level>          # (nếu mode=EVE)
<bot2_level>          # (nếu mode=EVE)
<row> <col>           # nước đi của người chơi (lặp lại)
```

**Ví dụ PVP:**
```
5
4
1
0 0
1 1
0 1
1 2
0 2
1 3
0 3
```

**Ví dụ EVE (bot vs bot):**
```
5
4
3
1
2
```

---

## 🤖 Grader

```bash
python grader.py
```

Script tự động chạy `game.exe` với các test case và so sánh output.

---

## 📄 License

Project học thuật — không dùng cho mục đích thương mại.
