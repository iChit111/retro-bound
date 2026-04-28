#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>

// ----------------------UTILS.H-----------------------

char h_edge = 205,
	v_edge = 186,
	lt_cor = 201,
	rt_cor = 187,
	lb_cor = 200,
	rb_cor = 188;

void genCh(int x, int y, char c)
{
	gotoxy(x, y);
	cprintf("%c", c);
	return;
}

void genNum(int x, int y, int n)
{
    gotoxy(x, y);
    cout << n;
    return;
}

void genSen(int x, int y, char *s)
{
    gotoxy(x, y);
    cout << s;
    return;
}

void horLine(int x1, int x2, int y)
{
	gotoxy(x1, y);
	for(int i = 0; i <= (x2 - x1); i++) cout << h_edge;
	return;
}

void vertLine(int y1, int y2, int x)
{
	for(int i = y1; i <= y2; i++) genCh(x, i, v_edge);
	return;
}

void drawBox(int x1, int y1, int x2, int y2)
{
    genCh(x1, y1, lt_cor);
    horLine(x1 + 1, x2 - 1, y1);
    genCh(x2, y1, rt_cor);
    vertLine(y1 + 1, y2 - 1, x2);
    genCh(x1, y2, lb_cor);
    horLine(x1 + 1, x2 - 1, y2);
    genCh(x2, y2, rb_cor);
    vertLine(y1 + 1, y2 - 1, x1);
	return;
}

void setColor(int text, int bg) {
    textcolor(text);
    textbackground(bg);
}

// ----------------------RETRO.H-------------------

class Interactable;

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

        void displayPlayer()
        {
            char p;
            switch(facing) {
                case UP:    p = '^'; break;
                case DOWN:  p = 'v'; break;
                case RIGHT: p = '>'; break;
                case LEFT:  p = '<'; break;
            }
            textcolor(YELLOW);
            genCh(pos.x, pos.y, p);
            textcolor(WHITE);
        }

        void updateOrientation(short action, coordinate b1, coordinate b2, Interactable **items, int numItems);

        state getState()
        {
            state s;
            s.pos = pos;
            s.hp = hp;
            s.facing = facing;
            return s;
        }
};

class Interactable
{
    private:
        coordinate pos;
        char icon;
        char* message;
        int canInteractFrom[4];

    public:
        Interactable(short x, short y, char c, char* msg, int u, int l, int r, int d)
        {
            pos.x = x; pos.y = y;
            icon = c;
            message = msg;
            canInteractFrom[UP] = u;
            canInteractFrom[LEFT] = l;
            canInteractFrom[RIGHT] = r;
            canInteractFrom[DOWN] = d;
        }

        void display()
        {
            textcolor(GREEN);
            genCh(pos.x, pos.y, icon);
            textcolor(WHITE);
        }

        coordinate getPos() { return pos; }

        int tryInteract(coordinate playerPos, short playerFacing)
        {
            if (playerPos.x == pos.x && playerPos.y == pos.y + 1
                && playerFacing == UP && canInteractFrom[DOWN]) return 1;
            if (playerPos.x == pos.x && playerPos.y == pos.y - 1
                && playerFacing == DOWN && canInteractFrom[UP]) return 1;
            if (playerPos.x == pos.x + 2 && playerPos.y == pos.y
                && playerFacing == LEFT && canInteractFrom[RIGHT]) return 1;
            if (playerPos.x == pos.x - 2 && playerPos.y == pos.y
                && playerFacing == RIGHT && canInteractFrom[LEFT]) return 1;

            return 0;
        }

        void showMessage()
        {
            genSen(2, 22, "                                        ");
            genSen(2, 22, message);
        }
};

// Defined outside class since Interactable must be fully defined first
void Player::updateOrientation(short action, coordinate b1, coordinate b2, Interactable **items, int numItems)
{
    coordinate oldPos = pos;
    int nextX = pos.x;
    int nextY = pos.y;

    switch (action) {
        case UP:
            if (facing == UP) nextY--;
            else facing = UP;
            break;
        case LEFT:
            if (facing == LEFT) nextX -= 2;
            else facing = LEFT;
            break;
        case RIGHT:
            if (facing == RIGHT) nextX += 2;
            else facing = RIGHT;
            break;
        case DOWN:
            if (facing == DOWN) nextY++;
            else facing = DOWN;
            break;   
    }

    if (oldPos.x != nextX || oldPos.y != nextY) {
        if (nextX >= b1.x && nextX <= b2.x && nextY >= b1.y && nextY <= b2.y) {
            int isBlocked = 0;
            for (int i = 0; i < numItems; i++)
            {
                if (items[i] -> getPos().x == nextX && items[i] -> getPos().y == nextY) {
                    isBlocked = 1;
                    break;
                }
            }

            if (!isBlocked)
            {
                pos.x = nextX;
                pos.y = nextY;
            }
        }
    }

    // Only erase old cell if player actually moved
    if (oldPos.x != pos.x || oldPos.y != pos.y) {
        genCh(oldPos.x, oldPos.y, ' ');
    }
}

class Retro
{
    private:
        coordinate boundary1, boundary2;
        state currentState;
        Player *player;
        Interactable *items[10];
        int numItems;

        void displayUI()
        {
            drawBox(1, 1, 79, 24);
            horLine(2, 66, 20);
            vertLine(1, 24, 67);
            genCh(67, 1, 203);
            genCh(67, 24, 202);
			genCh(1, 20, 204);
			genCh(67, 20, 185);
            genSen(68, 2, "Retro-Bound");
            gotoxy(69, 3);
            for(int i = 0; i < 10; i++) cout << (char)196;
        }

    public:
        Retro()
        {
            boundary1.x = 2; boundary1.y = 2;
            boundary2.x = 66; boundary2.y = 19;
            currentState = loadState();
            setColor(WHITE, BLACK);
            player = new Player(currentState);
            numItems = 0;
            items[numItems++] = new Interactable(12, 10, 'A', "This is a character", 1, 1, 1, 1);
            items[numItems++] = new Interactable(12, 11, 'B', "This is another character", 0, 1, 1, 1);
        }

        void run()
        {
            clrscr();
            displayUI();
            for (int i = 0; i < numItems; i++) items[i]->display();
            player->displayPlayer();

            char ch;
            do
            {
                ch = getch();
                short action = -1;

                switch (ch) {
                    case 'w': action = UP;    break;
                    case 'a': action = LEFT;  break;
                    case 's': action = DOWN;  break;
                    case 'd': action = RIGHT; break;
                    case 'e':
                        for (int i = 0; i < numItems; i++)
                        {
                            if (items[i] -> tryInteract(player -> getState().pos, player -> getState().facing)) {
                                items[i] -> showMessage();
                                break;
                            }
                        }
                        break;
                }

                if (action != -1) {
                    player->updateOrientation(action, boundary1, boundary2, items, numItems);

                    for (int i = 0; i < numItems; i++) items[i]->display();

                    player->displayPlayer();
                }

            } while (ch != 'q');
        }

        ~Retro()
        {
            currentState = player->getState();
            saveState(currentState);
            for (int i = 0; i < numItems; i++)
            {
                delete items[i];
            }
            delete player;
        }
};

state loadState()
{
    state temp;
    temp.pos.x = 40; temp.pos.y = 12;
    temp.hp = 100; temp.facing = DOWN;

    ifstream f("C:\\TURBOC3\\Projects\\SAVE.DAT");
    if (f)
    {
        f >> temp.pos.x >> temp.pos.y >> temp.facing;
        f.close();
    }
    return temp;
}

void saveState(state s)
{
    ofstream f("C:\\TURBOC3\\Projects\\SAVE.DAT");
    if (f)
    {
        f << s.pos.x << "\n" << s.pos.y << "\n" << s.facing;
        f.close();
    }
}

int main() {
    Retro Game;
    Game.run();
    return 0;
}