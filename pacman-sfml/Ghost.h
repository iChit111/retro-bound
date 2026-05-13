#pragma once
#include <SFML/Graphics.hpp>
#include "Maze.h"

// Forward declare so Ghost.h doesn't depend on full Pacman.h
class Pacman;

// ─── Ghost Modes ──────────────────────────────────────────────────────────────
// Each ghost cycles between SCATTER (roam corner) → CHASE (hunt Pac-Man)
// → FRIGHTENED (after a power pellet) → EATEN (return to ghost house)
enum class GhostMode { SCATTER, CHASE, FRIGHTENED, EATEN };

// ─── Ghost Names / Personalities ─────────────────────────────────────────────
enum class GhostName { BLINKY, PINKY, INKY, CLYDE };

// ─── Ghost Class ──────────────────────────────────────────────────────────────
class Ghost {
public:
    Ghost(GhostName name, sf::Color color, int startCol, int startRow);

    void update(float dt, const Maze& maze, const Pacman& pac,
                const Ghost& blinky); // Inky needs Blinky's pos

    void draw(sf::RenderWindow& window);

    // Called by Game when Pac-Man eats a power pellet
    void frighten();

    // Called by Game when this ghost is eaten while frightened
    void eaten();

    // Has this ghost just touched Pac-Man? (checked by Game)
    bool collidesWith(const Pacman& pac) const;

    bool isFrightened() const { return m_mode == GhostMode::FRIGHTENED; }
    bool isEaten()      const { return m_mode == GhostMode::EATEN;      }

    int  getCol() const;
    int  getRow() const;

    void reset();

    // Points for eating this ghost (doubles per ghost per pellet: 200/400/800/1600)
    int  getEatScore() const { return m_eatScore; }
    void resetEatScore()     { m_eatScore = 200;  }

private:
    GhostName    m_name;
    sf::Color    m_color;
    sf::Vector2f m_pos;        // pixel centre
    sf::Vector2f m_startPos;

    int   m_dir;               // current direction (uses Pacman::RIGHT etc.)
    float m_speed;
    GhostMode m_mode;
    float m_frightenTimer;     // countdown while frightened
    float m_flashTimer;        // for flashing white near end of frighten
    bool  m_flashing;
    int   m_eatScore;

    // Each ghost has a fixed scatter target (corner of the maze)
    int m_scatterCol, m_scatterRow;

    // Compute this ghost's chase target tile based on personality
    sf::Vector2i getChaseTarget(const Pacman& pac, const Ghost& blinky) const;

    // BFS/Manhattan-distance next direction toward a target tile
    int  chooseDirection(const Maze& maze, int targetCol, int targetRow) const;

    // Move one step; can't reverse direction (core Pac-Man rule)
    void move(float dt, const Maze& maze, int targetCol, int targetRow);

    // Draw the ghost body (dome + skirt + eyes + pupils)
    void drawBody(sf::RenderWindow& window, sf::Color bodyColor);

    // Return the correct eyes direction vector based on m_dir
    sf::Vector2f eyeOffset() const;

    static const float NORMAL_SPEED;
    static const float EATEN_SPEED;
    static const float FRIGHTEN_SPEED;
    static const float FRIGHTEN_DURATION;
    static const float FLASH_START;
};
