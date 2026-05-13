#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// ─── Tile Types ───────────────────────────────────────────────────────────────
// Each cell in the maze is one of these values.
enum TileType {
    WALL       = 0,   // solid blue wall
    DOT        = 1,   // small dot Pac-Man eats
    POWER      = 2,   // large power pellet (makes ghosts scared)
    EMPTY      = 3,   // open floor, already eaten
    GHOST_DOOR = 4    // the one-way door above the ghost house
};

// ─── Maze Constants ───────────────────────────────────────────────────────────
const int TILE_SIZE  = 24;   // pixels per tile
const int MAZE_COLS  = 28;   // classic Pac-Man width
const int MAZE_ROWS  = 31;   // classic Pac-Man height

// ─── Maze Class ───────────────────────────────────────────────────────────────
// Responsible for:
//   • Storing and drawing the tile grid
//   • Tracking dots remaining
//   • Providing collision queries
class Maze {
public:
    Maze();

    void draw(sf::RenderWindow& window);

    // Returns true if the tile at grid (col, row) blocks movement
    bool isWall(int col, int row) const;

    // Returns true if the tile is the ghost-house door
    bool isGhostDoor(int col, int row) const;

    // Eat the dot/pellet at (col, row).
    // Returns 10 for a dot, 50 for a power pellet, 0 if nothing there.
    int  eatTile(int col, int row);

    // True when all dots and pellets have been eaten
    bool allDotsEaten() const;

    // How many dots are left (used for Elroy mode trigger)
    int  dotsRemaining() const { return m_dotsLeft; }

    // Reset maze to its original state (new life / new level)
    void reset();

    // Convert pixel position → grid column/row
    static int pixelToCol(float px) { return (int)(px / TILE_SIZE); }
    static int pixelToRow(float py) { return (int)(py / TILE_SIZE); }

    // Convert grid column/row → pixel centre of that tile
    static float colToPixel(int col) { return col * TILE_SIZE + TILE_SIZE / 2.f; }
    static float rowToPixel(int row) { return row * TILE_SIZE + TILE_SIZE / 2.f; }

private:
    // The live grid (dots get replaced with EMPTY as they are eaten)
    std::vector<std::vector<int>> m_grid;

    int m_dotsLeft = 0;

    // Renders a single wall tile with a rounded-rectangle style
    void drawWallTile(sf::RenderWindow& w, int col, int row);

    // Pre-built SFML shapes for dots and pellets (created once, reused)
    sf::CircleShape m_dotShape;
    sf::CircleShape m_pelletShape;
    sf::RectangleShape m_wallShape;
    sf::RectangleShape m_doorShape;
    sf::Clock          m_pelletClock;  // drives pellet flashing animation

    void buildShapes();
    void countDots();
};
