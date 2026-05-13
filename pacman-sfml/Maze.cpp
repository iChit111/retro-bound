#include "Maze.h"
#include <SFML/Graphics.hpp>

// ─── Classic 28×31 Pac-Man maze layout ───────────────────────────────────────
// 0 = wall, 1 = dot, 2 = power pellet, 3 = empty, 4 = ghost door
static const int BASE_MAZE[MAZE_ROWS][MAZE_COLS] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,2,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,2,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,3,0,0,3,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,3,0,0,3,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,3,3,3,3,3,3,3,3,3,3,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,3,0,0,0,4,4,0,0,0,3,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,3,0,3,3,3,3,3,3,0,3,0,0,1,0,0,0,0,0,0},
    {3,3,3,3,3,3,1,3,3,3,0,3,3,3,3,3,3,0,3,3,3,1,3,3,3,3,3,3},
    {0,0,0,0,0,0,1,0,0,3,0,3,3,3,3,3,3,0,3,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,3,0,0,0,0,0,0,0,0,3,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,3,3,3,3,3,3,3,3,3,3,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,3,0,0,0,0,0,0,0,0,3,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,3,0,0,0,0,0,0,0,0,3,0,0,1,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,2,1,1,0,0,1,1,1,1,1,1,1,3,3,1,1,1,1,1,1,1,0,0,1,1,2,0},
    {0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0},
    {0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0},
    {0,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

// ─── Constructor ─────────────────────────────────────────────────────────────
Maze::Maze() {
    buildShapes();
    reset();
}

// Copy the static layout into our live grid and count dots
void Maze::reset() {
    m_grid.assign(MAZE_ROWS, std::vector<int>(MAZE_COLS));
    for (int r = 0; r < MAZE_ROWS; r++)
        for (int c = 0; c < MAZE_COLS; c++)
            m_grid[r][c] = BASE_MAZE[r][c];
    countDots();
}

void Maze::countDots() {
    m_dotsLeft = 0;
    for (int r = 0; r < MAZE_ROWS; r++)
        for (int c = 0; c < MAZE_COLS; c++)
            if (m_grid[r][c] == DOT || m_grid[r][c] == POWER)
                m_dotsLeft++;
}

// ─── Shape setup (called once) ────────────────────────────────────────────────
void Maze::buildShapes() {
    // Small white dot
    m_dotShape.setRadius(2.f);
    m_dotShape.setFillColor(sf::Color(255, 255, 200));
    m_dotShape.setOrigin(2.f, 2.f);

    // Larger flashing pellet
    m_pelletShape.setRadius(5.f);
    m_pelletShape.setFillColor(sf::Color(255, 255, 200));
    m_pelletShape.setOrigin(5.f, 5.f);

    // Wall tile (filled in draw loop)
    m_wallShape.setSize(sf::Vector2f((float)TILE_SIZE, (float)TILE_SIZE));
    m_wallShape.setFillColor(sf::Color(33, 33, 222));     // classic blue
    m_wallShape.setOutlineColor(sf::Color(100, 100, 255));
    m_wallShape.setOutlineThickness(1.f);

    // Ghost-house door (pink horizontal bar)
    m_doorShape.setSize(sf::Vector2f((float)TILE_SIZE * 2, 4.f));
    m_doorShape.setFillColor(sf::Color(255, 182, 193));
}

// ─── Draw ─────────────────────────────────────────────────────────────────────
void Maze::draw(sf::RenderWindow& window) {
    // Use elapsed time to make pellets flash
    float t = m_pelletClock.getElapsedTime().asSeconds();
    bool pelletVisible = (int)(t * 2) % 2 == 0; // toggles ~2x per second

    for (int r = 0; r < MAZE_ROWS; r++) {
        for (int c = 0; c < MAZE_COLS; c++) {
            float px = (float)(c * TILE_SIZE);
            float py = (float)(r * TILE_SIZE);

            switch (m_grid[r][c]) {
                case WALL:
                    m_wallShape.setPosition(px, py);
                    window.draw(m_wallShape);
                    break;

                case DOT:
                    m_dotShape.setPosition(px + TILE_SIZE / 2.f, py + TILE_SIZE / 2.f);
                    window.draw(m_dotShape);
                    break;

                case POWER:
                    if (pelletVisible) {
                        m_pelletShape.setPosition(px + TILE_SIZE / 2.f, py + TILE_SIZE / 2.f);
                        window.draw(m_pelletShape);
                    }
                    break;

                case GHOST_DOOR:
                    // Draw a thin pink door across two tiles
                    m_doorShape.setPosition(px, py + TILE_SIZE / 2.f - 2.f);
                    window.draw(m_doorShape);
                    break;

                default: break; // EMPTY — nothing to draw
            }
        }
    }
}

// ─── Queries ──────────────────────────────────────────────────────────────────
bool Maze::isWall(int col, int row) const {
    if (col < 0 || col >= MAZE_COLS || row < 0 || row >= MAZE_ROWS)
        return true; // treat out-of-bounds as wall (except tunnel row handled in Game)
    return m_grid[row][col] == WALL;
}

bool Maze::isGhostDoor(int col, int row) const {
    if (col < 0 || col >= MAZE_COLS || row < 0 || row >= MAZE_ROWS) return false;
    return m_grid[row][col] == GHOST_DOOR;
}

int Maze::eatTile(int col, int row) {
    if (col < 0 || col >= MAZE_COLS || row < 0 || row >= MAZE_ROWS) return 0;
    int tile = m_grid[row][col];
    if (tile == DOT) {
        m_grid[row][col] = EMPTY;
        m_dotsLeft--;
        return 10;
    }
    if (tile == POWER) {
        m_grid[row][col] = EMPTY;
        m_dotsLeft--;
        return 50;
    }
    return 0;
}

bool Maze::allDotsEaten() const {
    return m_dotsLeft <= 0;
}
