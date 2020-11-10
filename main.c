#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include "fabled.h"

// following struct defined in "fabled.h"
// struct fabledStatus {
//     unsigned status;       // 0: quit -- 1: command -- 2: editing
//     unsigned x, y;         // (x, y) for edit area
//     char srcname[81]       // name of blocks file on disk
//     char (*s)[1024];       // screens (1024 chars each)
//     unsigned ns;           // number of screens
//     unsigned current;      // current screen
//     char message[80];      // message
// };

int main(int argc, char **argv) {
    struct fabledStatus s[1] = { 1, 4, 5, "blocks.fb", 0, 0, 0, "" };
    if (argc != 1) strcpy(s->srcname, argv[1]);
    if (!loadsource(s)) {
        // start ncurses
        initscr();
        cbreak();
        keypad(stdscr, TRUE);

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

        // end ncurses
        move(24, 0);
        endwin();
        freescreens(s);
    }
    if (*s->msg) printf("%s\n", s->msg);
    return 0;
}