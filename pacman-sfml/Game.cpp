#include "Game.h"
#include <sstream>

// ─── Window / Layout ─────────────────────────────────────────────────────────
static const int WIN_W  = MAZE_COLS * TILE_SIZE;           // 672 px
static const int WIN_H  = MAZE_ROWS * TILE_SIZE + 60;      // 744 px + HUD strip
static const int HUD_Y  = MAZE_ROWS * TILE_SIZE;           // y where HUD starts

// Classic scatter/chase timing (seconds)
static const float SCATTER_DURATION = 7.f;
static const float CHASE_DURATION   = 20.f;
static const int   MAX_LIVES        = 3;
static const int   MAX_LEVELS       = 3;

// ─── Constructor ─────────────────────────────────────────────────────────────
Game::Game()
    : m_window(sf::VideoMode({static_cast<unsigned int>(WIN_W), static_cast<unsigned int>(WIN_H)}), "Pac-Man  |  Modern C++ with SFML",
               sf::Style::Titlebar | sf::Style::Close)
    , m_blinky(GhostName::BLINKY, sf::Color(255,  0,  0), 14,  11)  // Red
    , m_pinky (GhostName::PINKY,  sf::Color(255,184,255), 14,  14)  // Pink
    , m_inky  (GhostName::INKY,   sf::Color(  0,255,255), 12,  14)  // Cyan
    , m_clyde (GhostName::CLYDE,  sf::Color(255,184,  0), 16,  14)  // Orange
{
    m_window.setFramerateLimit(60);

    // Load a simple system font as fallback
    if (!m_font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        // The compiler requires us to check the result.
        // We leave this block empty so it safely does nothing if it fails.
    }

    m_state          = GameState::MENU;
    m_score          = 0;
    m_lives          = MAX_LIVES;
    m_level          = 1;
    m_ghostEatCombo  = 1;
    m_stateTimer     = 0.f;
    m_globalModeTimer= 0.f;
    m_inChaseMode    = false;
    m_flashTimer     = 0.f;
    m_flashVisible   = true;
}

// ─── Main Loop ────────────────────────────────────────────────────────────────
void Game::run() {
    while (m_window.isOpen()) {
        float dt = m_clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f; // clamp to avoid spiral of death

        processEvents();
        update(dt);
        render();
    }
}

// ─── Event Processing ────────────────────────────────────────────────────────
// ─── Event Processing ────────────────────────────────────────────────────────
void Game::processEvents() {
    // SFML 3: pollEvent now returns a std::optional
    while (const std::optional event = m_window.pollEvent()) {
        
        // SFML 3: Check for closed window using the new type-safe 'is' method
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        // SFML 3: Extract the key press data using 'getIf'
        if (const auto* keyPress = event->getIf<sf::Event::KeyPressed>()) {
            
            // Menu → start game
            if (m_state == GameState::MENU && keyPress->code == sf::Keyboard::Key::Enter) {
                m_state = GameState::PLAYING;
            }
            
            // Game Over / Win → back to menu
            if ((m_state == GameState::GAME_OVER || m_state == GameState::YOU_WIN)
                && keyPress->code == sf::Keyboard::Key::Enter) {
                // Full reset
                m_score = 0; m_lives = MAX_LIVES; m_level = 1;
                m_maze.reset();
                m_pac.reset();
                m_blinky.reset(); m_pinky.reset(); m_inky.reset(); m_clyde.reset();
                m_state = GameState::MENU;
            }

            // Movement keys (Note the added 'Key::' for the SFML 3 scoped enums)
            if (m_state == GameState::PLAYING) {
                if (keyPress->code == sf::Keyboard::Key::W || keyPress->code == sf::Keyboard::Key::Up)
                    m_pac.setDesiredDir(Pacman::UP);
                if (keyPress->code == sf::Keyboard::Key::S || keyPress->code == sf::Keyboard::Key::Down)
                    m_pac.setDesiredDir(Pacman::DOWN);
                if (keyPress->code == sf::Keyboard::Key::A || keyPress->code == sf::Keyboard::Key::Left)
                    m_pac.setDesiredDir(Pacman::LEFT);
                if (keyPress->code == sf::Keyboard::Key::D || keyPress->code == sf::Keyboard::Key::Right)
                    m_pac.setDesiredDir(Pacman::RIGHT);
            }
        }
    }
}
// ─── Update Dispatcher ────────────────────────────────────────────────────────
void Game::update(float dt) {
    // Flash timer (used for blinking text)
    m_flashTimer += dt;
    if (m_flashTimer > 0.4f) { m_flashTimer = 0.f; m_flashVisible = !m_flashVisible; }

    switch (m_state) {
        case GameState::PLAYING:   updatePlaying(dt);  break;
        case GameState::DYING:     updateDying(dt);    break;
        case GameState::LEVEL_WIN: updateLevelWin(dt); break;
        default: break;
    }
}

// ─── Main Gameplay Update ─────────────────────────────────────────────────────
void Game::updatePlaying(float dt) {
    // ── Global scatter ↔ chase mode cycling ──────────────────────────────
    m_globalModeTimer += dt;
    float modeDuration = m_inChaseMode ? CHASE_DURATION : SCATTER_DURATION;
    if (m_globalModeTimer >= modeDuration) {
        m_globalModeTimer = 0.f;
        m_inChaseMode = !m_inChaseMode;
        switchGhostModes(m_inChaseMode);
    }

    // ── Move Pac-Man ──────────────────────────────────────────────────────
    m_pac.update(dt, m_maze);

    // ── Eat dots / pellets ────────────────────────────────────────────────
    int pts = m_maze.eatTile(m_pac.getCol(), m_pac.getRow());
    if (pts == 50) {
        // Power pellet eaten: frighten all ghosts
        m_score += pts;
        m_ghostEatCombo = 1;
        m_blinky.frighten(); m_pinky.frighten();
        m_inky.frighten();   m_clyde.frighten();
        m_blinky.resetEatScore(); m_pinky.resetEatScore();
        m_inky.resetEatScore();   m_clyde.resetEatScore();
    } else if (pts > 0) {
        m_score += pts;
    }

    // ── Move ghosts ───────────────────────────────────────────────────────
    m_blinky.update(dt, m_maze, m_pac, m_blinky);
    m_pinky .update(dt, m_maze, m_pac, m_blinky);
    m_inky  .update(dt, m_maze, m_pac, m_blinky);
    m_clyde .update(dt, m_maze, m_pac, m_blinky);

    // ── Check collisions ──────────────────────────────────────────────────
    checkGhostCollisions();

    // ── Level complete? ───────────────────────────────────────────────────
    if (m_maze.allDotsEaten()) {
        m_state      = GameState::LEVEL_WIN;
        m_stateTimer = 2.5f;
    }
}

// ─── Ghost Collision Logic ────────────────────────────────────────────────────
void Game::checkGhostCollisions() {
    Ghost* ghosts[4] = { &m_blinky, &m_pinky, &m_inky, &m_clyde };

    for (Ghost* g : ghosts) {
        if (!g->collidesWith(m_pac)) continue;

        if (g->isFrightened()) {
            // Eat the ghost
            int pts = 200 * m_ghostEatCombo;
            m_score      += pts;
            m_ghostEatCombo *= 2;
            g->eaten();
        } else if (!g->isEaten()) {
            // Pac-Man dies
            m_pac.die();
            m_state      = GameState::DYING;
            m_stateTimer = 2.0f; // wait for death animation
        }
    }
}

// ─── Dying State Update ───────────────────────────────────────────────────────
void Game::updateDying(float dt) {
    m_pac.update(dt, m_maze); // keep death animation running
    m_stateTimer -= dt;
    if (m_stateTimer <= 0.f) {
        m_lives--;
        if (m_lives <= 0) {
            m_state = GameState::GAME_OVER;
        } else {
            // Respawn
            m_pac.reset();
            m_blinky.reset(); m_pinky.reset();
            m_inky.reset();   m_clyde.reset();
            m_state = GameState::PLAYING;
        }
    }
}

// ─── Level Win State Update ───────────────────────────────────────────────────
void Game::updateLevelWin(float dt) {
    m_stateTimer -= dt;
    if (m_stateTimer <= 0.f) {
        m_level++;
        if (m_level > MAX_LEVELS) {
            m_state = GameState::YOU_WIN;
        } else {
            m_maze.reset();
            m_pac.reset();
            m_blinky.reset(); m_pinky.reset();
            m_inky.reset();   m_clyde.reset();
            m_state = GameState::PLAYING;
        }
    }
}

// ─── Switch ghost modes globally ─────────────────────────────────────────────
void Game::switchGhostModes(bool toChase) {
    // Don't override frightened or eaten ghosts
    // (The ghost's own update handles exiting those modes)
}

// ─── Render ───────────────────────────────────────────────────────────────────
void Game::render() {
    m_window.clear(sf::Color::Black);

    if (m_state == GameState::MENU) {
        drawMenu();
    } else {
        m_maze.draw(m_window);

        if (m_state != GameState::DYING || !m_pac.isDead())
            m_pac.draw(m_window);

        m_blinky.draw(m_window);
        m_pinky .draw(m_window);
        m_inky  .draw(m_window);
        m_clyde .draw(m_window);

        drawHUD();
        drawLives();

        if (m_state == GameState::LEVEL_WIN)
            drawOverlay("LEVEL COMPLETE!", "Get ready...", sf::Color::Cyan);
        if (m_state == GameState::GAME_OVER)
            drawOverlay("GAME OVER", "Press Enter to restart", sf::Color::Red);
        if (m_state == GameState::YOU_WIN)
            drawOverlay("YOU WIN!", "Press Enter for menu", sf::Color::Yellow);
    }

    m_window.display();
}

// ─── HUD (score / level) ──────────────────────────────────────────────────────
void Game::drawHUD() {
    sf::Text t(m_font);
    t.setCharacterSize(18);
    t.setFillColor(sf::Color::White);

    // Score
    std::ostringstream ss;
    ss << "SCORE: " << m_score;
    t.setString(ss.str());
    t.setPosition({8.f, (float)HUD_Y + 8.f});
    m_window.draw(t);

    // Level
    ss.str("");
    ss << "LEVEL: " << m_level;
    t.setString(ss.str());
    t.setPosition({(float)WIN_W / 2.f - 40.f, (float)HUD_Y + 8.f});
    m_window.draw(t);
}

// ─── Lives display (small Pac-Man icons) ─────────────────────────────────────
void Game::drawLives() {
    sf::CircleShape icon(7.f);
    icon.setFillColor(sf::Color::Yellow);
    for (int i = 0; i < m_lives - 1; i++) {
        icon.setPosition({(float)(WIN_W - 30 - i * 20), (float)HUD_Y + 10.f});
        m_window.draw(icon);
    }
}

// ─── Menu Screen ─────────────────────────────────────────────────────────────
void Game::drawMenu() {
    // Background
    sf::RectangleShape bg({ (float)WIN_W, (float)WIN_H });
    bg.setFillColor(sf::Color::Black);
    m_window.draw(bg);

    sf::Text title(m_font);
    title.setCharacterSize(52);
    title.setFillColor(sf::Color::Yellow);
    title.setString("PAC-MAN");
    centreText(title, 100.f);
    m_window.draw(title);

    sf::Text sub(m_font);
    sub.setCharacterSize(18);
    sub.setFillColor(sf::Color(200, 200, 200));
    sub.setString("Modern C++ with SFML");
    centreText(sub, 165.f);
    m_window.draw(sub);

    // Controls
    sf::Text ctrl(m_font);
    ctrl.setCharacterSize(16);
    ctrl.setFillColor(sf::Color(180, 180, 255));
    ctrl.setString("WASD / Arrow keys to move\nE to interact with objects");
    centreText(ctrl, 260.f);
    m_window.draw(ctrl);

    // Blinking "Press Enter"
    if (m_flashVisible) {
        sf::Text start(m_font);
        start.setCharacterSize(22);
        start.setFillColor(sf::Color::Cyan);
        start.setString("Press ENTER to Start");
        centreText(start, 360.f);
        m_window.draw(start);
    }

    // Ghost legend
    struct GhostInfo { sf::Color col; std::string name; std::string trait; };
    GhostInfo infos[4] = {
        { sf::Color::Red,                    "Blinky", "Direct chaser" },
        { sf::Color(255, 184, 255),          "Pinky",  "Ambushes ahead" },
        { sf::Color::Cyan,                   "Inky",   "Flanker" },
        { sf::Color(255, 184, 0),            "Clyde",  "Shy chaser" }
    };

    for (int i = 0; i < 4; i++) {
        sf::CircleShape ghost(10.f);
        ghost.setFillColor(infos[i].col);
        float gx = WIN_W / 2.f - 80.f + i * 50.f;
        ghost.setPosition({gx - 10.f, 430.f});
        m_window.draw(ghost);

        sf::Text nm(m_font);
        nm.setCharacterSize(11);
        nm.setFillColor(infos[i].col);
        nm.setString(infos[i].name + "\n" + infos[i].trait);
        nm.setPosition({gx - 20.f, 455.f});
        m_window.draw(nm);
    }
}

// ─── Overlay (LEVEL COMPLETE / GAME OVER / YOU WIN) ─────────────────────────
void Game::drawOverlay(const std::string& line1, const std::string& line2, sf::Color col) {
    // Semi-transparent dark box
    sf::RectangleShape box({ 360.f, 100.f });
    box.setFillColor(sf::Color(0, 0, 0, 180));
    box.setOrigin({180.f, 50.f});
    box.setPosition({(float)WIN_W / 2.f, (float)WIN_H / 2.f});
    m_window.draw(box);

    sf::Text t1(m_font);
    t1.setCharacterSize(30);
    t1.setFillColor(col);
    t1.setString(line1);
    centreText(t1, (float)WIN_H / 2.f - 40.f);
    m_window.draw(t1);

    if (m_flashVisible) {
        sf::Text t2(m_font);
        t2.setCharacterSize(18);
        t2.setFillColor(sf::Color::White);
        t2.setString(line2);
        centreText(t2, (float)WIN_H / 2.f + 5.f);
        m_window.draw(t2);
    }
}

void Game::centreText(sf::Text& t, float y) {
    sf::FloatRect r = t.getLocalBounds();
    // Use SFML 3's position and size properties
    t.setOrigin({r.position.x + r.size.x / 2.f, r.position.y});
    t.setPosition({(float)WIN_W / 2.f, y});
}