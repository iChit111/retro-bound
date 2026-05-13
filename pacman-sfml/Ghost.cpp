#include "Ghost.h"
#include "Pacman.h"
#include <cmath>
#include <climits>

// ─── Speed / Timing Constants ────────────────────────────────────────────────
const float Ghost::NORMAL_SPEED      = 75.f;   // px/s
const float Ghost::EATEN_SPEED       = 150.f;  // rush back to house
const float Ghost::FRIGHTEN_SPEED    = 45.f;   // slow when scared
const float Ghost::FRIGHTEN_DURATION = 8.f;    // seconds of fright
const float Ghost::FLASH_START       = 2.f;    // start flashing 2s before end

// Ghost-house door tile (col 13-14, row 12) — return destination when eaten
static const int HOUSE_COL = 13;
static const int HOUSE_ROW = 14;

// ─── Constructor ─────────────────────────────────────────────────────────────
Ghost::Ghost(GhostName name, sf::Color color, int startCol, int startRow)
    : m_name(name), m_color(color), m_eatScore(200)
{
    m_startPos = { Maze::colToPixel(startCol), Maze::rowToPixel(startRow) };

    // Scatter corner targets (classic Pac-Man corners)
    switch (name) {
        case GhostName::BLINKY: m_scatterCol = 25; m_scatterRow =  0; break;
        case GhostName::PINKY:  m_scatterCol =  2; m_scatterRow =  0; break;
        case GhostName::INKY:   m_scatterCol = 27; m_scatterRow = 30; break;
        case GhostName::CLYDE:  m_scatterCol =  0; m_scatterRow = 30; break;
    }
    reset();
}

void Ghost::reset() {
    m_pos          = m_startPos;
    m_dir          = Pacman::LEFT;
    m_mode         = GhostMode::SCATTER;
    m_speed        = NORMAL_SPEED;
    m_frightenTimer= 0.f;
    m_flashTimer   = 0.f;
    m_flashing     = false;
    m_eatScore     = 200;
}

int Ghost::getCol() const { return Maze::pixelToCol(m_pos.x); }
int Ghost::getRow() const { return Maze::pixelToRow(m_pos.y); }

// ─── Frighten / Eat ───────────────────────────────────────────────────────────
void Ghost::frighten() {
    if (m_mode == GhostMode::EATEN) return; // already dead, ignore
    m_mode          = GhostMode::FRIGHTENED;
    m_frightenTimer = FRIGHTEN_DURATION;
    m_flashing      = false;
    m_speed         = FRIGHTEN_SPEED;
}

void Ghost::eaten() {
    m_mode  = GhostMode::EATEN;
    m_speed = EATEN_SPEED;
}

// ─── Collision ────────────────────────────────────────────────────────────────
bool Ghost::collidesWith(const Pacman& pac) const {
    float dx = m_pos.x - pac.getPosition().x;
    float dy = m_pos.y - pac.getPosition().y;
    return std::sqrt(dx*dx + dy*dy) < (float)TILE_SIZE * 0.7f;
}

// ─── Personality: Chase Targets ───────────────────────────────────────────────
// This is what makes each ghost feel different!
sf::Vector2i Ghost::getChaseTarget(const Pacman& pac, const Ghost& blinky) const {
    int pc = pac.getCol(), pr = pac.getRow();
    int pd = pac.getFacing(); // we'll cast through Pacman

    switch (m_name) {
        case GhostName::BLINKY:
            // Blinky always targets Pac-Man's exact tile — pure chaser
            return { pc, pr };

        case GhostName::PINKY: {
            // Pinky targets 4 tiles ahead of Pac-Man (ambush)
            int tc = pc, tr = pr;
            if      (pd == Pacman::UP)    tr -= 4;
            else if (pd == Pacman::DOWN)  tr += 4;
            else if (pd == Pacman::LEFT)  tc -= 4;
            else if (pd == Pacman::RIGHT) tc += 4;
            return { tc, tr };
        }

        case GhostName::INKY: {
            // Inky: midpoint between Blinky and 2 tiles ahead of Pac-Man,
            // then double the distance — creates a flanking effect
            int tc = pc, tr = pr;
            if      (pd == Pacman::UP)    tr -= 2;
            else if (pd == Pacman::DOWN)  tr += 2;
            else if (pd == Pacman::LEFT)  tc -= 2;
            else if (pd == Pacman::RIGHT) tc += 2;

            int bx = blinky.getCol(), by = blinky.getRow();
            return { 2*tc - bx, 2*tr - by };
        }

        case GhostName::CLYDE: {
            // Clyde chases when far (>8 tiles), scatters when close — shy!
            float dx = (float)(getCol() - pc);
            float dy = (float)(getRow() - pr);
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 8.f)
                return { pc, pr };          // chase
            else
                return { m_scatterCol, m_scatterRow }; // run to corner
        }
    }
    return { pc, pr }; // fallback
}

// ─── Direction Chooser (Manhattan distance to target) ────────────────────────
// Pac-Man ghost rule: ghosts cannot reverse direction (except when
// switching modes). They pick the open neighbour closest to the target.
int Ghost::chooseDirection(const Maze& maze, int targetCol, int targetRow) const {
    // Opposite directions (ghosts can't reverse)
    auto opposite = [](int d) {
        if (d == Pacman::RIGHT) return Pacman::LEFT;
        if (d == Pacman::LEFT)  return Pacman::RIGHT;
        if (d == Pacman::UP)    return Pacman::DOWN;
        return Pacman::UP;
    };

    int dirs[4]  = { Pacman::UP, Pacman::LEFT, Pacman::DOWN, Pacman::RIGHT };
    int dcs[4]   = {  0, -1,  0,  1 };
    int drs[4]   = { -1,  0,  1,  0 };

    int bestDir  = m_dir;
    int bestDist = INT_MAX;

    for (int i = 0; i < 4; i++) {
        if (dirs[i] == opposite(m_dir)) continue; // no reversing
        int nc = getCol() + dcs[i];
        int nr = getRow() + drs[i];
        if (maze.isWall(nc, nr)) continue;
        // Ghost can pass through ghost door when returning (EATEN mode)
        if (maze.isGhostDoor(nc, nr) && m_mode != GhostMode::EATEN) continue;

        int dist = std::abs(nc - targetCol) + std::abs(nr - targetRow);
        if (dist < bestDist) {
            bestDist = dist;
            bestDir  = dirs[i];
        }
    }
    return bestDir;
}

// ─── Movement ────────────────────────────────────────────────────────────────
void Ghost::move(float dt, const Maze& maze, int targetCol, int targetRow) {
    // Decide direction only when near the centre of a tile
    float cx = Maze::colToPixel(getCol());
    float cy = Maze::rowToPixel(getRow());
    const float SNAP = m_speed * dt + 2.f;

    if (std::abs(m_pos.x - cx) < SNAP && std::abs(m_pos.y - cy) < SNAP) {
        m_pos.x = cx; m_pos.y = cy; // snap to centre
        m_dir = chooseDirection(maze, targetCol, targetRow);
    }

    // Advance in chosen direction
    float dx = 0, dy = 0;
    if      (m_dir == Pacman::RIGHT) dx =  m_speed * dt;
    else if (m_dir == Pacman::LEFT)  dx = -m_speed * dt;
    else if (m_dir == Pacman::UP)    dy = -m_speed * dt;
    else if (m_dir == Pacman::DOWN)  dy =  m_speed * dt;

    int nc = getCol() + (dx > 0 ? 1 : dx < 0 ? -1 : 0);
    int nr = getRow() + (dy > 0 ? 1 : dy < 0 ? -1 : 0);
    bool blocked = maze.isWall(nc, nr) ||
                   (maze.isGhostDoor(nc, nr) && m_mode != GhostMode::EATEN);

    if (!blocked) {
        m_pos.x += dx;
        m_pos.y += dy;
    }

    // Tunnel wrap
    if (m_pos.x < 0)                              m_pos.x = (float)(MAZE_COLS * TILE_SIZE);
    if (m_pos.x > (float)(MAZE_COLS * TILE_SIZE)) m_pos.x = 0;
}

// ─── Update ───────────────────────────────────────────────────────────────────
void Ghost::update(float dt, const Maze& maze, const Pacman& pac, const Ghost& blinky) {
    // Handle frightened timer
    if (m_mode == GhostMode::FRIGHTENED) {
        m_frightenTimer -= dt;
        if (m_frightenTimer <= FLASH_START) m_flashing = true;
        if (m_frightenTimer <= 0.f) {
            m_mode   = GhostMode::SCATTER;
            m_speed  = NORMAL_SPEED;
            m_flashing = false;
        }
    }

    // Pick target based on mode
    int targetCol, targetRow;
    switch (m_mode) {
        case GhostMode::CHASE: {
            auto t = getChaseTarget(pac, blinky);
            targetCol = t.x; targetRow = t.y;
            break;
        }
        case GhostMode::SCATTER:
            targetCol = m_scatterCol; targetRow = m_scatterRow;
            break;
        case GhostMode::FRIGHTENED:
            // Random wandering — target a tile far away
            targetCol = (getCol() + 7) % MAZE_COLS;
            targetRow = (getRow() + 7) % MAZE_ROWS;
            break;
        case GhostMode::EATEN:
            targetCol = HOUSE_COL; targetRow = HOUSE_ROW;
            // Once back home, revive
            if (std::abs(m_pos.x - Maze::colToPixel(HOUSE_COL)) < 4.f &&
                std::abs(m_pos.y - Maze::rowToPixel(HOUSE_ROW)) < 4.f) {
                m_mode  = GhostMode::SCATTER;
                m_speed = NORMAL_SPEED;
            }
            break;
    }

    move(dt, maze, targetCol, targetRow);
}

// ─── Draw ─────────────────────────────────────────────────────────────────────
void Ghost::draw(sf::RenderWindow& window) {
    sf::Color bodyColor = m_color;

    if (m_mode == GhostMode::FRIGHTENED) {
        if (!m_flashing) {
            bodyColor = sf::Color(0, 0, 200); // dark blue when scared
        } else {
            // Alternate white and blue
            float t = m_frightenTimer;
            bodyColor = (int)(t * 4) % 2 == 0 ? sf::Color::White : sf::Color(0, 0, 200);
        }
    }
    if (m_mode == GhostMode::EATEN) {
        // Just draw eyes (body is invisible)
        drawBody(window, sf::Color(0, 0, 0, 0));
        return;
    }

    drawBody(window, bodyColor);
}

// ─── Ghost Body Drawing ───────────────────────────────────────────────────────
// Dome (semicircle) + rectangular skirt with 3 bumps on bottom + 2 eyes
void Ghost::drawBody(sf::RenderWindow& window, sf::Color bodyColor) {
    float r   = 10.f;
    float px  = m_pos.x;
    float py  = m_pos.y;

    bool showBody = (bodyColor.a > 0);

    if (showBody) {
        // ── Dome ─────────────────────────────────────────────
        sf::CircleShape dome(r);
        dome.setFillColor(bodyColor);
        dome.setOrigin(r, r);
        dome.setPosition(px, py - 2.f);
        window.draw(dome);

        // ── Body rectangle ────────────────────────────────────
        sf::RectangleShape body({ r * 2.f, r });
        body.setFillColor(bodyColor);
        body.setOrigin(r, 0.f);
        body.setPosition(px, py - 2.f);
        window.draw(body);

        // ── Skirt bumps (3 small semicircles along bottom) ────
        float bumpR = r / 3.f;
        for (int i = 0; i < 3; i++) {
            sf::CircleShape bump(bumpR);
            bump.setFillColor(bodyColor);
            bump.setOrigin(bumpR, bumpR);
            bump.setPosition(px - r + bumpR + i * (r * 2.f / 3.f) + bumpR, py + r - 2.f);
            window.draw(bump);
        }
    }

    // ── Eyes ──────────────────────────────────────────────────
    if (m_mode != GhostMode::FRIGHTENED) {
        float eyeOffX = 4.f;
        float eyeOffY = -4.f;
        for (int side = -1; side <= 1; side += 2) {
            // White of eye
            sf::CircleShape eyeWhite(3.5f);
            eyeWhite.setFillColor(sf::Color::White);
            eyeWhite.setOrigin(3.5f, 3.5f);
            eyeWhite.setPosition(px + side * eyeOffX, py + eyeOffY);
            window.draw(eyeWhite);

            // Pupil (shifts toward movement direction)
            sf::Vector2f pupilOff = eyeOffset();
            sf::CircleShape pupil(2.f);
            pupil.setFillColor(sf::Color(0, 0, 200));
            pupil.setOrigin(2.f, 2.f);
            pupil.setPosition(px + side * eyeOffX + pupilOff.x, py + eyeOffY + pupilOff.y);
            window.draw(pupil);
        }
    } else {
        // Scared face: two dot eyes and a wavy mouth line
        sf::CircleShape eye(2.f);
        eye.setFillColor(sf::Color::White);
        eye.setOrigin(2.f, 2.f);
        eye.setPosition(px - 3.f, py - 3.f); window.draw(eye);
        eye.setPosition(px + 3.f, py - 3.f); window.draw(eye);
    }
}

sf::Vector2f Ghost::eyeOffset() const {
    switch (m_dir) {
        case Pacman::RIGHT: return {  2.f,  0.f };
        case Pacman::LEFT:  return { -2.f,  0.f };
        case Pacman::UP:    return {  0.f, -2.f };
        case Pacman::DOWN:  return {  0.f,  2.f };
    }
    return { 0.f, 0.f };
}
