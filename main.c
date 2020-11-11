#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "able.h"

int main(int argc, char **argv) {
    struct ableInfo s[1] = { 0 };
    strcpy(s->srcname, "blocks.fb");
    if (argc != 1) strcpy(s->srcname, argv[1]);

    if (loadsource(s)) {
        fprintf(stderr, "Error: %s\n", s->msg);
        exit(EXIT_FAILURE);
    }

    // start ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();

    mvprintw(0, 3, "Welcome to ABLE - (A)nother (BL)ock (E)ditor - v0.1.1 (2020-11-11)");

    window_setup(s);
    refreshall(s);
    getch();

#if 0
    if (!loadsource(s)) {

        // work
        addframe(s);
        refresh_curpage(s);
        while (s->status) {
            switch (s->status) {
                default: // unrecognized status, shouldn't happen
                         flash();
                         s->status = 0;
                         strcpy(s->msg, "(unrecognized status)");
                         break;
                case 0: // quit, doesn't happen
                        break;
                case 1: // command-line
                        mvprintw(22, 6, "                        ");
                        move(22, 6);
                        echo();
                        char cmd[25];
                        getnstr(cmd, 24);
                        docmd(s, cmd);
                        break;
                case 2: // editing
                        mvprintw(22, 6, "Hit <TAB> for command   ");
                        move(s->y, s->x);
                        noecho();
                        edit(s);
                        break;
            }
        }

        freescreens(s);
    }
#endif

    window_destroy(s);

    // end ncurses
    move(24, 0);
    endwin();

    if (*s->msg) printf("%s\n", s->msg);
    return 0;
}
