#ifndef UTILS_H
#define UTILS_H

#include <iostream.h>
#include <conio.h>

char h_edge = 205, v_edge = 186, lt_cor = 201, rt_cor = 187, lb_cor = 200, rb_cor = 188;

void genCh(int x, int y, char c) {
    gotoxy(x, y);
    cprintf("%c", c);
}
// ... Move genSen, genNum, horLine, vertLine, drawBox here ...
void genSen(int x, int y, char *s) {
    gotoxy(x, y);
    cout << s;
}

void genNum(int x, int y, int n) {
    gotoxy(x, y);
    cout << n;
}

void horLine(int x1, int x2, int y) {
    gotoxy(x1, y);
    for(int i = 0; i <= (x2 - x1); i++) cout << h_edge;
}

void vertLine(int y1, int y2, int x) {
    for(int i = y1; i <= y2; i++) genCh(x, i, v_edge);
}

void drawBox(int x1, int y1, int x2, int y2) {
    genCh(x1, y1, lt_cor);
    horLine(x1 + 1, x2 - 1, y1);
    genCh(x2, y1, rt_cor);
    vertLine(y1 + 1, y2 - 1, x2);
    genCh(x1, y2, lb_cor);
    horLine(x1 + 1, x2 - 1, y2);
    genCh(x2, y2, rb_cor);
    vertLine(y1 + 1, y2 - 1, x1);
}

struct coordinate { short x, y; };
enum face { UP, LEFT, RIGHT, DOWN };

#endif