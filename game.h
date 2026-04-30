#ifndef GAME_H
#define GAME_H

#include "objects.h"
#include "player.h"

class Retro {
private:
    Player *player;
    GameObject *worldObjects[20]; // Expanded to hold up to 20 items per level
    int numObjects;
    int score;
    int currentLevel;
    int targetScore;
    int maxLevel;

    // Frees up dynamic memory before loading the next level
    void clearLevel() {
        if (player != 0) {
            delete player;
            player = 0;
        }
        for(int i = 0; i < numObjects; i++) {
            if (worldObjects[i] != 0) {
                delete worldObjects[i];
                worldObjects[i] = 0;
            }
        }
    }

    // Handles the object instantiation for each stage
    void loadLevel(int level) {
        clearLevel();
        currentLevel = level;
        score = 0;
        
        player = new Player(34, 12);
        
        if (level == 1) {
            targetScore = 3;
            numObjects = 6;
            worldObjects[0] = new Collectible(16, 15, 'O', RED);
            worldObjects[1] = new Collectible(34, 15, 'O', GREEN);
            worldObjects[2] = new Collectible(52, 15, 'O', BLUE);
            worldObjects[3] = new DepositBox(16, 4, RED);
            worldObjects[4] = new DepositBox(34, 4, GREEN);
            worldObjects[5] = new DepositBox(52, 4, BLUE);
        }
        else if (level == 2) {
            targetScore = 4;
            numObjects = 8;
            // Introducing new Turbo C++ colors: MAGENTA, YELLOW, CYAN
            worldObjects[0] = new Collectible(10, 15, 'O', MAGENTA);
            worldObjects[1] = new Collectible(26, 15, 'O', YELLOW);
            worldObjects[2] = new Collectible(42, 15, 'O', CYAN);
            worldObjects[3] = new Collectible(58, 15, 'O', RED);
            
            worldObjects[4] = new DepositBox(10, 4, MAGENTA);
            worldObjects[5] = new DepositBox(26, 4, YELLOW);
            worldObjects[6] = new DepositBox(42, 4, CYAN);
            worldObjects[7] = new DepositBox(58, 4, RED);
        }
        else if (level == 3) {
            targetScore = 5;
            numObjects = 10;
            // Mixing up the deposit box positions so it's not a straight line up!
            worldObjects[0] = new Collectible(12, 16, 'O', BLUE);
            worldObjects[1] = new Collectible(24, 16, 'O', RED);
            worldObjects[2] = new Collectible(36, 16, 'O', GREEN);
            worldObjects[3] = new Collectible(48, 16, 'O', YELLOW);
            worldObjects[4] = new Collectible(60, 16, 'O', MAGENTA);
            
            worldObjects[5] = new DepositBox(12, 4, GREEN);
            worldObjects[6] = new DepositBox(24, 4, YELLOW);
            worldObjects[7] = new DepositBox(36, 4, MAGENTA);
            worldObjects[8] = new DepositBox(48, 4, BLUE);
            worldObjects[9] = new DepositBox(60, 4, RED);
        }
    }

public:
    Retro() {
        player = 0;
        numObjects = 0;
        maxLevel = 3;
        for(int i = 0; i < 20; i++) worldObjects[i] = 0;
        loadLevel(1);
    }
    
    void run() {
        clrscr();
        coordinate b1; b1.x = 2; b1.y = 2;
        coordinate b2; b2.x = 66; b2.y = 19;
        char ch;
        
        do {
            drawBox(1, 1, 68, 20); 
            genSen(2, 1, "[ Task: Match Colors ]");
            
            // Updated HUD for levels and target scores
            genSen(2, 21, "Level: "); genNum(9, 21, currentLevel);
            genSen(18, 21, "Score: "); genNum(25, 21, score);
            genSen(32, 21, "Target: "); genNum(40, 21, targetScore);

            for(int i=0; i < numObjects; i++) {
                worldObjects[i]->draw();
            }
            
            player->draw();
            
            ch = getch();
            short act = -1;
            
            if (ch == 'w') act = UP;
            else if (ch == 'a') act = LEFT;
            else if (ch == 's') act = DOWN;
            else if (ch == 'd') act = RIGHT;
            else if (ch == 'e') {
                for(int i = 0; i < numObjects; i++) {
                    int points = worldObjects[i]->interact(player);
                    if (points > 0) {
                        score += points;
                        worldObjects[i]->draw();
                        genNum(25, 21, score); 
                    }
                }
            }
            
            if (act != -1) {
                player->move(act, b1, b2);
            }
            
            // Level progression logic
            if (score == targetScore) {
                genSen(22, 10, " LEVEL COMPLETE! ");
                getch();
                
                if (currentLevel < maxLevel) {
                    loadLevel(currentLevel + 1);
                    clrscr(); // Refresh the board graphics for the new objects
                } else {
                    genSen(21, 12, " YOU BEAT THE GAME! ");
                    getch();
                    break;
                }
            }
            
        } while (ch != 'q');
    }
    
    ~Retro() {
        clearLevel();
    }
};

#endif