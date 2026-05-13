cmake_minimum_required(VERSION 3.16)
project(PacManSFML CXX)

# ── C++ Standard ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Source Files ─────────────────────────────────────────────────────────────
set(SOURCES
    main.cpp
    Game.cpp
    Maze.cpp
    Pacman.cpp
    Ghost.cpp
)

add_executable(pacman ${SOURCES})

# ── Find SFML ─────────────────────────────────────────────────────────────────
# Option A (recommended): install SFML via your package manager, e.g.
#   Ubuntu/Debian : sudo apt install libsfml-dev
#   Arch Linux    : sudo pacman -S sfml
#   macOS         : brew install sfml
#   Windows       : use vcpkg → vcpkg install sfml
#
# Option B: set SFML_DIR to the cmake folder inside your SFML installation
# set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
target_link_libraries(pacman PRIVATE SFML::Graphics SFML::Window SFML::System)

# ── Compiler Warnings (good practice!) ───────────────────────────────────────
if (MSVC)
    target_compile_options(pacman PRIVATE /W3)
else()
    target_compile_options(pacman PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(WIN32)
    add_custom_command(TARGET pacman POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:pacman>
            $<TARGET_FILE_DIR:pacman>
        COMMAND_EXPAND_LISTS
    )
endif()