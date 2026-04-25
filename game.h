#ifndef GAME_H
#define GAME_H

#include "objects.h"
#include "player.h"

class Retro {
private:
    Player *player;
    GameObject *worldObjects[6];
    int score;
public:
    Retro() {
        score = 0;
        player = new Player(34, 12);
        
        worldObjects[0] = new Collectible(16, 15, 'O', RED);
        worldObjects[1] = new Collectible(34, 15, 'O', GREEN);
        worldObjects[2] = new Collectible(52, 15, 'O', BLUE);
        
        worldObjects[3] = new DepositBox(16, 4, RED);
        worldObjects[4] = new DepositBox(34, 4, GREEN);
        worldObjects[5] = new DepositBox(52, 4, BLUE);
    }
    
    void run() {
        clrscr();
        coordinate b1; b1.x = 2; b1.y = 2;
        coordinate b2; b2.x = 66; b2.y = 19;
        char ch;
        
        do {
            drawBox(1, 1, 68, 20); 
            genSen(2, 1, "[ Task: Match Colors ]");
            genSen(2, 21, "Score: "); genNum(9, 21, score);

            for(int i=0; i<6; i++) {
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
                for(int i = 0; i < 6; i++) {
                    int points = worldObjects[i]->interact(player);
                    if (points > 0) {
                        score += points;
                        worldObjects[i]->draw();
                        // Update the score display immediately so the player sees it
                        genNum(9, 21, score); 
                    }
                }
            }
            
            if (act != -1) {
                player->move(act, b1, b2);
            }
            
            if (score == 3) {
                genSen(25, 10, " LEVEL COMPLETE! ");
                getch();
                break;
            }
            
        } while (ch != 'q');
    }
    
    ~Retro() {
        delete player;
        for(int i=0; i<6; i++) {
            delete worldObjects[i];
        }
    }
};


#endif