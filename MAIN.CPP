#include <iostream.h>
#include <conio.h>
#include <string.h>

// Constants
char h_edge = 205,
	v_edge = 186,
	lt_cor = 201,
	rt_cor = 187,
	lb_cor = 200,
	rb_cor = 188;
struct coordinate
{
	int x, y;
};

coordinate boundary1 = {2, 2}, boundary2 = {78, 23};

// Movement function
void genCh(int x, int y, char c)
{
	gotoxy(x, y);
	cout << c;
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
    gotoxy(x1, y1);
    genCh(x1, y1, lt_cor);
    horLine(2, 79, 1);
    genCh(x2, y1, rt_cor);
    vertLine(2, 23, x2);
    gotoxy(x1, y2);
    genCh(1, 24, lb_cor);
    horLine(2, 79, y2);
    genCh(x2, y2, rb_cor);
    gotoxy(x1, y1);
    vertLine(2, 23, 1);
	return;
}

int main()
{
    coordinate pos = {40, 12};
    clrscr();
    char ch, player = 'v';

    // Drawing grids
    drawBox(1, 1, 80, 24);
    genSen(35, 2, "Retro-Bound");

    // Player loop
    genCh(pos.x, pos.y, player);
    do
    {
	    ch = getch();
	    switch (ch)
	    {
		    case 'w':
			    player = '^';
			    if (pos.y > boundary1.y) pos.y--;
			    break;
		    case 'a':
			    player = '<';
			    if (pos.x > boundary1.x) pos.x -= 2;
			    break;
		    case 's':
			    player = 'v';
			    if (pos.y < boundary2.y) pos.y++;
			    break;
		    case 'd':
			    player = '>';
			    if (pos.x < boundary2.x) pos.x += 2;
			    break;
	    }
	    cout << "\b ";
	    genCh(pos.x, pos.y, player);
    } while (ch != 'q');

    return 0;
}