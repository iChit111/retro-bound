#include <iostream.h>
#include <conio.h>

struct coordinate
{
	int x, y;
};

coordinate boundary1 = {2, 2}, boundary2 = {78, 22};

// Movement function
void genCh(coordinate p, char c)
{
	gotoxy(p.x, p.y);
	cout << c;
}

int main()
{
	coordinate pos = {41, 12};
    clrscr();
    char ch, player, lt_cor = 201, h_edge = 205, rt_cor = 187, v_edge = 186, lb_cor = 200, rb_cor = 188;
    gotoxy(1, 1);
    cout << lt_cor;

    for (int i = 2; i < 79; i++)
    {
        gotoxy(i, 1);
        cout << h_edge;

        gotoxy(i, 23);
        cout << h_edge;
    }
    gotoxy(79, 1);
    cout << rt_cor;

    for (i = 2; i < 23; i++)
    {
        gotoxy(1, i);
        cout << v_edge;

        gotoxy(79, i);
        cout << v_edge;
    }

    gotoxy(79, 23);
    cout << rb_cor;
    gotoxy(1, 23);
    cout << lb_cor;

    gotoxy(pos.x, pos.y);

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
			    if (pos.x > boundary1.x) pos.x--;
			    break;
		    case 's':
			    player = 'v';
			    if (pos.y < boundary2.y) pos.y++;
			    break;
		    case 'd':
			    player = '>';
			    if (pos.x < boundary2.x) pos.x++;
			    break;
	    }

	    cout << "\b ";
	    genCh(pos, player);
    } while (ch != 'q');

    return 0;
}