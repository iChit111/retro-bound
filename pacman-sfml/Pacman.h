#pragma once
#include <SFML/Graphics.hpp>
#include "Maze.h"

// ─── Pacman Class ─────────────────────────────────────────────────────────────
// Handles:
//   • Smooth pixel-level movement with grid-snapping
//   • Mouth open/close animation
//   • Tunnel wrapping (left ↔ right edges)
//   • Death animation
class Pacman {
public:
    // Starting pixel position is the classic spawn point (col 14, row 23)
    Pacman();

    void update(float dt, const Maze& maze);
    void draw(sf::RenderWindow& window);

    // Queue the next desired direction (set by keyboard input in Game.cpp)
    void setDesiredDir(int dir);  // 0=RIGHT 1=LEFT 2=UP 3=DOWN

    // Reset to spawn position (called on new life or new level)
    void reset();

    // Grid position of the tile Pac-Man currently occupies
    int getCol() const;
    int getRow() const;
    int getFacing() const { return m_dir; }  // used by Ghost AI (Pinky/Inky)

    // Pixel centre position (used for collision math)
    sf::Vector2f getPosition() const;

    // Is Pac-Man dying right now?
    bool isDying()  const { return m_dying; }
    bool isDead()   const { return m_dead; }

    // Trigger the death animation
    void die();

    // Direction constants — shared with Ghost.h
    static const int RIGHT = 0;
    static const int LEFT  = 1;
    static const int UP    = 2;
    static const int DOWN  = 3;
    static const int NONE  = -1;

private:
    sf::Vector2f m_pos;        // pixel centre
    int m_dir;                 // current movement direction
    int m_desiredDir;          // buffered next direction

    float m_speed;             // pixels per second
    float m_mouthAngle;        // degrees open (animates 0–45)
    float m_mouthDir;          // +1 opening, -1 closing
    float m_rotation;          // sprite rotation to face direction

    bool m_dying;
    bool m_dead;
    float m_dyingTimer;        // counts up during death animation
    float m_dyingAngle;        // chomping-wide animation angle

    sf::CircleShape m_body;

    // Returns true if Pac-Man is close enough to the centre of his
    // current tile to safely turn (prevents clipping into walls)
    bool canTurn(const Maze& maze, int newDir) const;

    // Try to move one step; returns false if wall blocks
    bool tryMove(const Maze& maze, int dir, float dt);

    void updateAnimation(float dt);
    void drawNormal(sf::RenderWindow& window);
    void drawDying(sf::RenderWindow& window);
};
