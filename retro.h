#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include "utils.h"

struct coordinate
{
	short x, y;
};

enum face {UP, LEFT, RIGHT, DOWN};

struct state
{
    coordinate pos;
    short hp, status, facing;
};

state loadState();
void saveState(state);
class Player;

class Retro
{
    private:
        coordinate boundary1, boundary2;
        state currentState;
        Player player;

        void displayUI()
        {
            drawBox(1, 1, 79, 24);
            vertLine(1, 24, 67);
            genCh(67, 1, 203);
            genCh(67, 24, 202);
            genSen(68, 2, "Retro-Bound");
            gotoxy(68, 3);
            for(int i = 0; i < 11; i++) cout << (char)196;
        }

    public:
        Retro()
        {
            boundary1.x = 2; boundary1.y = 2;
            boundary2.x = 66; boundary2.y = 23;
            currentState = loadState(); 
        }

        void run()
        {
            clrscr();
            displayUI();
            char ch;
            player.displayPlayer();
            do
            {
                ch = getch();
                short action;
                switch (ch) {
                    case 'w':
                        action = UP;
                        break;
                    case 'a':
                        action = LEFT;
                        break;
                    case 's':
                        action = RIGHT;
                        break;
                    case 'd':
                        action = DOWN;
                        break;
                }
                player.updatePlayer(action, boundary1, boundary2);
            } while (ch != 'q');
        }
        
        ~Retro()
        {
            saveState(currentState);
        }
};

class Player
{
    coordinate pos;
    short hp, facing;

    public:
        Player(state s)
        {
            pos.x = s.pos.x;
            pos.y = s.pos.y;
            hp = s.hp;
            facing = s.facing;
        }

        void displayPlayer() {
            char p;

            switch(facing) {
                case UP:
                    p = '^';
                    break;
                case DOWN:
                    p = 'v';
                    break;
                case RIGHT:
                    p = '>';
                    break;
                case LEFT:
                    p = '<';
                    break;
            }
            cout << "\b ";
            genCh(pos.x, pos.y, player);
        }

        void updateOrientation(short action, coordinate b1, coordinate b2) {
            switch (action){
                case UP:
                    if(facing == UP) if (pos.y > b1.y) pos.y--;
                    else facing = UP;
                    break;
                case LEFT:
                    if(facing == LEFT) if (pos.x > b1.x) pos.x--;
                    else facing = LEFT;
                    break;
                case RIGHT:
                    if(facing == RIGHT) if (pos.x > b2.x) pos.x++;
                    else facing = RIGHT;
                    break;
                case DOWN:
                    if(facing == DOWN) if (pos.y < b2.y) pos.y++;
                    else facing = DOWN;
                    break;
            }
        }

        friend Retro :: readPlayer();
};


// Movement function

state loadState()
{
    // default
    state temp;
    
    ifstream f("save.dat");
    if (f)
    {
        f >> temp.pos.x >> temp.pos.y;
        f.close();
    }

    return temp;
}
void saveState(state s)
{
    ofstream f("save.dat");
    if (f)
    {
        f << s.pos.x << "\n" << s.pos.y;
        f.close();
    }
}