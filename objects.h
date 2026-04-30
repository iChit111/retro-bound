#ifndef OBJECTS_H
#define OBJECTS_H

#include "utils.h"
#include "player.h"

class GameObject {
protected:
    coordinate pos;
    int color;
    char icon;
public:
    GameObject(short x, short y, char c, int col) {
        pos.x = x; pos.y = y;
        icon = c; color = col;
    }
    virtual void draw() {
        textcolor(color);
        genCh(pos.x, pos.y, icon);
        textcolor(WHITE);
    }
    virtual int interact(Player* p) { return 0; }
    virtual ~GameObject() {} 
};

class Collectible : public GameObject {
private:
    int isPickedUp;
public:
    Collectible(short x, short y, char c, int col) : GameObject(x, y, c, col) {
        isPickedUp = 0;
    }
    virtual void draw() {
        if (!isPickedUp) GameObject::draw();
    }
    virtual int interact(Player* p) {
        coordinate pp = p->getPos();
        if (!isPickedUp && pp.x == pos.x && pp.y == pos.y && p->getHeldColor() == 0) {
            isPickedUp = 1;
            p->hold(color);
            genSen(2, 22, "Picked up the item!     ");
        }
        return 0;
    }
};

class DepositBox : public GameObject {
private:
    int isFilled;
public:
    DepositBox(short x, short y, int col) : GameObject(x, y, 'H', col) {
        isFilled = 0;
    }
    // In objects.h
    virtual int interact(Player* p) {
        coordinate pp = p->getPos();
        if (!isFilled && pp.x == pos.x && pp.y == pos.y + 1 && p->getFacing() == UP) {
            if (p->getHeldColor() == color) {
                isFilled = 1; // Mark as filled so this block never runs again
                icon = 'X';
                p->hold(0);
                genSen(2, 22, "Color Matched!          ");
                return 1; // Return the point
            }
        }
        return 0; // Return 0 if already filled or wrong color
    }
};

class MovingObstacle : public GameObject {
    int dx;
    coordinate b1, b2;
public:
    MovingObstacle(short x, short y, coordinate bound1, coordinate bound2)
        : GameObject(x, y, 'X', WHITE), dx(1), b1(bound1), b2(bound2) {}

    void update() {
        genCh(pos.x, pos.y, ' '); // erase
        pos.x += dx;
        if (pos.x >= b2.x || pos.x <= b1.x) dx = -dx;
    }

    virtual int interact(Player* p) {
        coordinate pp = p->getPos();
        if (pp.x == pos.x && pp.y == pos.y) {
            p->hold(0); // drop whatever you're carrying
            genSen(2, 22, "Dropped item!           ");
        }
        return 0;
    }
};

#endif