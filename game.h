// game.h
#ifndef GAME_H
#define GAME_H

#include "objects.h"
#include "player.h"

class Retro {
private:
    Player *player;
    GameObject *worldObjects[10]; // expand array for more objects
    int objectCount;
    int score;
    int totalScore;

    void clearObjects() {
        for (int i = 0; i < objectCount; i++) delete worldObjects[i];
        objectCount = 0;
    }

    void setupLevel(int level) {
        clearObjects();
        delete player;
        player = new Player(34, 12);

        if (level == 1) {
            // Original level — match 3 colors
            worldObjects[0] = new Collectible(16, 15, 'O', RED);
            worldObjects[1] = new Collectible(34, 15, 'O', GREEN);
            worldObjects[2] = new Collectible(52, 15, 'O', BLUE);
            worldObjects[3] = new DepositBox(16, 4, RED);
            worldObjects[4] = new DepositBox(34, 4, GREEN);
            worldObjects[5] = new DepositBox(52, 4, BLUE);
            objectCount = 6;
        }
        else if (level == 2) {
            // Level 2 — boxes are now on the sides, collectibles in the middle
            // Player must navigate more to match
            worldObjects[0] = new Collectible(34, 15, 'O', RED);
            worldObjects[1] = new Collectible(34, 10, 'O', GREEN);
            worldObjects[2] = new Collectible(34,  5, 'O', CYAN);
            worldObjects[3] = new DepositBox( 5, 10, RED);   // far left
            worldObjects[4] = new DepositBox(63, 10, GREEN); // far right
            worldObjects[5] = new DepositBox(34,  3, CYAN);  // top center
            objectCount = 6;
        }
        else if (level == 3) {
            // Level 3 — 4 colors, tighter layout, more objects to manage
            worldObjects[0] = new Collectible(10, 17, 'O', RED);
            worldObjects[1] = new Collectible(25, 17, 'O', GREEN);
            worldObjects[2] = new Collectible(43, 17, 'O', BLUE);
            worldObjects[3] = new Collectible(58, 17, 'O', CYAN);
            worldObjects[4] = new DepositBox(10,  3, RED);
            worldObjects[5] = new DepositBox(25,  3, GREEN);
            worldObjects[6] = new DepositBox(43,  3, BLUE);
            worldObjects[7] = new DepositBox(58,  3, CYAN);
            objectCount = 8;
        }
    }

    // Returns true if level is complete
    bool runLevel(int level, int targetScore) {
        clrscr();
        score = 0;
        coordinate b1; b1.x = 2; b1.y = 2;
        coordinate b2; b2.x = 66; b2.y = 19;
        char ch;

        do {
            drawBox(1, 1, 68, 20);
            genSen(2,  1, "[ Task: Match Colors ]");
            genSen(45, 1, "Level: "); genNum(52, 1, level);
            genSen(2, 21, "Score: "); genNum(9, 21, score);
            genSen(20, 21, "Total: "); genNum(27, 21, totalScore + score);

            for (int i = 0; i < objectCount; i++) worldObjects[i]->draw();
            player->draw();

            ch = getch();
            short act = -1;

            if      (ch == 'w') act = UP;
            else if (ch == 'a') act = LEFT;
            else if (ch == 's') act = DOWN;
            else if (ch == 'd') act = RIGHT;
            else if (ch == 'e') {
                for (int i = 0; i < objectCount; i++) {
                    int points = worldObjects[i]->interact(player);
                    if (points > 0) {
                        score += points;
                        worldObjects[i]->draw();
                        genNum(9, 21, score);
                        genNum(27, 21, totalScore + score);
                    }
                }
            }

            if (act != -1) player->move(act, b1, b2);

            if (score >= targetScore) {
                genSen(22, 10, " LEVEL COMPLETE! ");
                getch();
                totalScore += score;
                return true;
            }

        } while (ch != 'q');

        return false; // player quit
    }

public:
    Retro() : objectCount(0), score(0), totalScore(0) {
        player = new Player(34, 12);
    }

    void run() {
        for (int level = 1; level <= 3; level++) {
            setupLevel(level);
            bool completed = runLevel(level, level == 3 ? 4 : 3);
            if (!completed) break; // player quit mid-level

            if (level < 3) {
                clrscr();
                genSen(20, 11, "Get ready for the next level...");
                getch();
            }
        }

        // Game over / win screen
        clrscr();
        genSen(18, 10, " === GAME COMPLETE! === ");
        genSen(22, 12, "Final Score: ");
        genNum(35, 12, totalScore);
        getch();
    }

    ~Retro() {
        clearObjects();
        delete player;
    }
};

#endif