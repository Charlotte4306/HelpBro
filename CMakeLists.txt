cmake_minimum_required(VERSION 3.10)

# 1. Project name and C++ standard configuration
project(TicTacToe-LV2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Recursively find all source files in src/ and its subdirectories
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 3. Find SDL packages
list(PREPEND CMAKE_PREFIX_PATH "C:/SDL2")
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# 4. Define the executable target
add_executable(game ${SOURCES})

# 5. Add include directories
target_include_directories(game PRIVATE 
     src 
    ${SDL2_INCLUDE_DIRS}
)

# 6. Link libraries
target_link_libraries(game PRIVATE 
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
)

# 7. Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# 8. Compiler options
if(MSVC)
    target_compile_options(game PRIVATE 
        /W1                          # Chi hien thi loi nghiem trong
        /wd4065                      # switch co default nhung khong co case
        /wd4100                      # tham so ham khong duoc dung
        /wd4189                      # bien khai bao nhung khong dung
        /wd4244                      # chuyen doi kieu du lieu (int -> char...)
        /wd4267                      # chuyen doi size_t -> int
        /wd4996                      # ham deprecated
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Release>:/O2>
        /D_CRT_SECURE_NO_WARNINGS
    )
else()
    target_compile_options(game PRIVATE
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wno-unused-variable
        -O2
    )
endif()

# Copy assets/ to build/
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:game>/assets
)

# Copy SDL2 DLLs to output directory
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/SDL2/DLL/SDL2.dll"
        $<TARGET_FILE_DIR:game>
)
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/SDL2/DLL/SDL2_ttf.dll"
        $<TARGET_FILE_DIR:game>
)
