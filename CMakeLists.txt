cmake_minimum_required(VERSION 3.15)
project(game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB IMGUI_SOURCES imgui/*.cpp)

add_executable(game ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(game PRIVATE
    src
    src/LevelEditor
    src/Logging
    src/Inputs
    src/Rendering
    src/SaveLevel
    C:/raylib/raylib/build/raylib/include
    ${CMAKE_SOURCE_DIR}/imgui
)

target_link_directories(game PRIVATE
    C:/raylib/raylib/build/raylib
)

target_link_libraries(game PRIVATE raylib winmm opengl32 gdi32)
