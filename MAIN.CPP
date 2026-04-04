#include "retro.h"

int main()
{
    Retro Game;
    Game.run();
    getch();
    // Player loop
    // genCh(pos.x, pos.y, player);
    // do
    // {
	//     ch = getch();
	//     switch (ch)
	//     {
	// 	    case 'w':
	// 		    player = '^';
	// 		    if (pos.y > boundary1.y) pos.y--;
	// 		    break;
	// 	    case 'a':
	// 		    player = '<';
	// 		    if (pos.x > boundary1.x) pos.x -= 2;
	// 		    break;
	// 	    case 's':
	// 		    player = 'v';
	// 		    if (pos.y < boundary2.y) pos.y++;
	// 		    break;
	// 	    case 'd':
	// 		    player = '>';
	// 		    if (pos.x < boundary2.x) pos.x += 2;
	// 		    break;
	//     }
	//     cout << "\b ";
	//     genCh(pos.x, pos.y, player);
    // } while (ch != 'q');

    return 0;
}