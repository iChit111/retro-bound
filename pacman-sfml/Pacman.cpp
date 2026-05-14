#include "Pacman.h"
#include <cmath>

// Pac-Man starts at column 14, row 23 (classic spawn)
static const float SPAWN_X = Maze::colToPixel(14);
static const float SPAWN_Y = Maze::rowToPixel(23);
static const float PAC_SPEED = 90.f;  // pixels/second
static const float PAC_RADIUS = 10.f;

// ─── Constructor ─────────────────────────────────────────────────────────────
Pacman::Pacman() {
    m_body.setRadius(PAC_RADIUS);
    m_body.setOrigin({PAC_RADIUS, PAC_RADIUS});
    m_body.setFillColor(sf::Color::Yellow);
    reset();
}

void Pacman::reset() {
    m_pos        = { SPAWN_X, SPAWN_Y };
    m_dir        = LEFT;
    m_desiredDir = LEFT;
    m_speed      = PAC_SPEED;
    m_mouthAngle = 22.f;
    m_mouthDir   = 1.f;
    m_rotation   = 0.f;
    m_dying      = false;
    m_dead       = false;
    m_dyingTimer = 0.f;
    m_dyingAngle = 45.f;
}

void Pacman::setDesiredDir(int dir) {
    m_desiredDir = dir;
}

int Pacman::getCol() const { return Maze::pixelToCol(m_pos.x); }
int Pacman::getRow() const { return Maze::pixelToRow(m_pos.y); }
sf::Vector2f Pacman::getPosition() const { return m_pos; }

// ─── Death Trigger ────────────────────────────────────────────────────────────
void Pacman::die() {
    m_dying      = true;
    m_dead       = false;
    m_dyingTimer = 0.f;
    m_dyingAngle = 45.f;
}

// ─── Update ───────────────────────────────────────────────────────────────────
void Pacman::update(float dt, const Maze& maze) {
    if (m_dying) {
        // Death animation: mouth opens wider and wider until fully closed (360°)
        m_dyingTimer += dt;
        m_dyingAngle += 120.f * dt;  // expand the gap
        if (m_dyingAngle >= 360.f) {
            m_dyingAngle = 360.f;
            m_dead = true;
        }
        return;
    }

    // ── Try to switch to desired direction ─────────────────────────────────
    if (m_desiredDir != m_dir && canTurn(maze, m_desiredDir)) {
        m_dir = m_desiredDir;
    }

    // ── Move in current direction ──────────────────────────────────────────
    tryMove(maze, m_dir, dt);

    // ── Tunnel wrapping: left/right edges of row 14 ────────────────────────
    if (m_pos.x < 0)
        m_pos.x = (float)(MAZE_COLS * TILE_SIZE);
    if (m_pos.x > (float)(MAZE_COLS * TILE_SIZE))
        m_pos.x = 0;

    // ── Set rotation so mouth faces movement direction ─────────────────────
    switch (m_dir) {
        case RIGHT: m_rotation =   0.f; break;
        case LEFT:  m_rotation = 180.f; break;
        case UP:    m_rotation = 270.f; break;
        case DOWN:  m_rotation =  90.f; break;
    }

    updateAnimation(dt);
}

// ─── Can we turn into newDir without hitting a wall? ──────────────────────────
bool Pacman::canTurn(const Maze& maze, int newDir) const {
    // Pac-Man must be within a small tolerance of the tile centre to turn
    float cx = Maze::colToPixel(getCol());
    float cy = Maze::rowToPixel(getRow());
    const float TOLERANCE = 4.f;
    if (std::abs(m_pos.x - cx) > TOLERANCE) return false;
    if (std::abs(m_pos.y - cy) > TOLERANCE) return false;

    // Check if the neighbouring tile in newDir is open
    int nc = getCol(), nr = getRow();
    if      (newDir == RIGHT) nc++;
    else if (newDir == LEFT)  nc--;
    else if (newDir == UP)    nr--;
    else if (newDir == DOWN)  nr++;

    return !maze.isWall(nc, nr);
}

// ─── Move one frame in dir; snaps to tile centre if wall ahead ───────────────
bool Pacman::tryMove(const Maze& maze, int dir, float dt) {
    float dx = 0, dy = 0;
    int nc = getCol(), nr = getRow();

    if      (dir == RIGHT) { dx =  m_speed * dt; nc++; }
    else if (dir == LEFT)  { dx = -m_speed * dt; nc--; }
    else if (dir == UP)    { dy = -m_speed * dt; nr--; }
    else if (dir == DOWN)  { dy =  m_speed * dt; nr++; }

    // Snap to tile centre on the perpendicular axis to avoid drift
    // Snap to tile centre on the perpendicular axis to avoid drift
    if (dir == RIGHT || dir == LEFT)
        m_pos.y = Maze::rowToPixel(getRow()); // snap Y
    else
        m_pos.x = Maze::colToPixel(getCol()); // snap X

    if (!maze.isWall(nc, nr)) {
        m_pos.x += dx;
        m_pos.y += dy;
        return true;
    } else {
        // BUG FIX: If there is a wall ahead, keep moving until we hit the exact centre of the current tile
        float cx = Maze::colToPixel(getCol());
        float cy = Maze::rowToPixel(getRow());
        
        if (dir == RIGHT) m_pos.x = std::min(m_pos.x + dx, cx);
        if (dir == LEFT)  m_pos.x = std::max(m_pos.x + dx, cx);
        if (dir == DOWN)  m_pos.y = std::min(m_pos.y + dy, cy);
        if (dir == UP)    m_pos.y = std::max(m_pos.y + dy, cy);
        
        return false;
    }
}

// ─── Animate the chomping mouth ───────────────────────────────────────────────
void Pacman::updateAnimation(float dt) {
    m_mouthAngle += m_mouthDir * 200.f * dt;  // degrees per second
    if (m_mouthAngle >= 45.f) { m_mouthAngle = 45.f; m_mouthDir = -1.f; }
    if (m_mouthAngle <=  0.f) { m_mouthAngle =  0.f; m_mouthDir =  1.f; }
}

// ─── Draw ─────────────────────────────────────────────────────────────────────
void Pacman::draw(sf::RenderWindow& window) {
    if (m_dying) { drawDying(window); return; }
    drawNormal(window);
}

// Draw Pac-Man as a yellow pie shape (circle with two lines cut out for mouth)
void Pacman::drawNormal(sf::RenderWindow& window) {
    // Draw using a convex polygon approximating a pie slice with a bite taken out
    const int SEGMENTS = 30;
    sf::ConvexShape pie;
    pie.setPointCount(SEGMENTS + 2);
    pie.setFillColor(sf::Color::Yellow);
    pie.setPosition(m_pos);
    pie.setRotation(sf::degrees(m_rotation));

    float startAngle = m_mouthAngle;          // degrees from 0
    float endAngle   = 360.f - m_mouthAngle;  // degrees from 0

    pie.setPoint(0, sf::Vector2f(0.f, 0.f)); // centre
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = startAngle + (endAngle - startAngle) * i / (float)SEGMENTS;
        float rad   = angle * 3.14159f / 180.f;
        pie.setPoint(i + 1, sf::Vector2f(
            std::cos(rad) * PAC_RADIUS,
            std::sin(rad) * PAC_RADIUS
        ));
    }
    window.draw(pie);

    // Draw eye
    sf::CircleShape eye(2.f);
    eye.setFillColor(sf::Color::Black);
    eye.setOrigin({2.f, 2.f});
    // Eye position offset relative to facing direction
    float eyeAngle = (90.f + m_rotation) * 3.14159f / 180.f;
    eye.setPosition({
        m_pos.x + std::cos(eyeAngle) * 5.f,
        m_pos.y + std::sin(eyeAngle) * 5.f - 3.f
    });
    window.draw(eye);
}

// Death animation: the mouth gap grows from 45° to 360° (completely eaten)
void Pacman::drawDying(sf::RenderWindow& window) {
    float half = m_dyingAngle / 2.f;
    float startAngle = half;
    float endAngle   = 360.f - half;

    if (endAngle <= startAngle) return; // fully consumed

    const int SEGMENTS = 30;
    sf::ConvexShape pie;
    pie.setPointCount(SEGMENTS + 2);
    pie.setFillColor(sf::Color::Yellow);
    pie.setPosition(m_pos);
    pie.setRotation(sf::degrees(m_rotation));

    pie.setPoint(0, sf::Vector2f(0.f, 0.f));
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = startAngle + (endAngle - startAngle) * i / (float)SEGMENTS;
        float rad   = angle * 3.14159f / 180.f;
        pie.setPoint(i + 1, sf::Vector2f(
            std::cos(rad) * PAC_RADIUS,
            std::sin(rad) * PAC_RADIUS
        ));
    }
    window.draw(pie);
}
