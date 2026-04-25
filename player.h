#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

class Player {
private:
    coordinate pos;
    short facing;
    int heldColor; 
public:
    Player(short x, short y) {
        pos.x = x; pos.y = y;
        facing = DOWN;
        heldColor = 0;
    }
    
    void draw() {
        char p;
        switch(facing) {
            case UP: p = '^'; break;
            case DOWN: p = 'v'; break;
            case LEFT: p = '<'; break;
            case RIGHT: p = '>'; break;
        }
        if (heldColor != 0) textcolor(heldColor);
        else textcolor(YELLOW);
        genCh(pos.x, pos.y, p);
        textcolor(WHITE);
    }
    
    void move(short action, coordinate b1, coordinate b2) {
        genCh(pos.x, pos.y, ' '); // Erase old position
        if (action == UP) { if (facing == UP && pos.y > b1.y) pos.y--; else facing = UP; }
        else if (action == DOWN) { if (facing == DOWN && pos.y < b2.y) pos.y++; else facing = DOWN; }
        else if (action == LEFT) { if (facing == LEFT && pos.x > b1.x) pos.x -= 2; else facing = LEFT; }
        else if (action == RIGHT) { if (facing == RIGHT && pos.x < b2.x) pos.x += 2; else facing = RIGHT; }
    }
    
    coordinate getPos() { return pos; }
    short getFacing() { return facing; }
    void hold(int col) { heldColor = col; }
    int getHeldColor() { return heldColor; }
};

#endif