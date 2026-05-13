#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "Maze.h"
#include "Pacman.h"
#include "Ghost.h"

// ─── Game States ──────────────────────────────────────────────────────────────
enum class GameState {
    MENU,        // title screen
    PLAYING,     // active gameplay
    DYING,       // pac-man death animation playing
    LEVEL_WIN,   // brief pause before next level
    GAME_OVER,   // all lives lost
    YOU_WIN      // all levels cleared
};

// ─── Game Class ───────────────────────────────────────────────────────────────
// Owns the window, all game objects, scoring, lives, and game-state machine.
class Game {
public:
    Game();
    void run(); // enter the main loop

private:
    // ── Core objects ────────────────────────────────────────────────────────
    sf::RenderWindow m_window;
    sf::Font         m_font;
    sf::Clock        m_clock;

    Maze             m_maze;
    Pacman           m_pac;

    // 4 classic ghosts: Blinky, Pinky, Inky, Clyde
    Ghost m_blinky, m_pinky, m_inky, m_clyde;

    // ── Game data ────────────────────────────────────────────────────────────
    GameState m_state;
    int       m_score;
    int       m_lives;
    int       m_level;
    int       m_ghostEatCombo;    // 1–4 per power pellet (200/400/800/1600)

    float     m_stateTimer;       // multi-purpose countdown for state transitions
    float     m_globalModeTimer;  // drives scatter ↔ chase switching
    bool      m_inChaseMode;      // false = scatter, true = chase

    // Flashing "LEVEL COMPLETE" / "GAME OVER" text timer
    float     m_flashTimer;
    bool      m_flashVisible;

    // ── Per-frame functions ──────────────────────────────────────────────────
    void processEvents();
    void update(float dt);
    void render();

    // ── Sub-update helpers ───────────────────────────────────────────────────
    void updatePlaying(float dt);
    void updateDying(float dt);
    void updateLevelWin(float dt);
    void checkGhostCollisions();
    void switchGhostModes(bool toChase);

    // ── Drawing helpers ──────────────────────────────────────────────────────
    void drawHUD();
    void drawMenu();
    void drawOverlay(const std::string& line1, const std::string& line2, sf::Color col);
    void drawLives();

    // Helper: centre text on screen at a given y
    void centreText(sf::Text& t, float y);
};
