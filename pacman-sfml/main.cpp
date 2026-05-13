#include "Game.h"

// ─────────────────────────────────────────────────────────────────────────────
// main.cpp  —  Entry point for the modern Pac-Man demo
//
// This file is intentionally tiny. All logic lives in Game, Maze, Pacman,
// and Ghost — keeping main.cpp clean is a good C++ habit.
// ─────────────────────────────────────────────────────────────────────────────
int main() {
    Game game;
    game.run();
    return 0;
}
