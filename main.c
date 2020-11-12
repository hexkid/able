#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include "able.h"

#define VERSION "v0.1."

#if 0
    unsigned status;       // 0: command -- 1: edit -- 2: quit
    unsigned editx, edity; // (x, y) for edit area
    unsigned cmdx, cmdy;   // (x, y) for commad prompt (y is constant)
    char srcname[81];      // name of blocks file on disk
    char (*s)[1024];       // screens
    unsigned ms, ns;       // number of screens: allocated, used
    unsigned current;      // current screen
    char msg[81];          // message
    WINDOW *wpage, *wsource, *wedit;
    WINDOW *wcmd, *wstatus, *winfo;
#endif

int main(int argc, char **argv) {
    struct ableInfo s[1] = { 0 };
    strcpy(s->srcname, "blocks.fb");
    if (argc != 1) strcpy(s->srcname, argv[1]);

    // start ncurses
    initscr();
    cbreak();
    noecho();
    start_color();

    #include "count.inc" // int count = ##; // compilation count :-)
    mvprintw(0, 3, "Welcome to ABLE - (A)nother (BL)ock (E)ditor - %s%d",
          VERSION, count);
    loadsource(s);
    windowscreate(s);

    do {
        refreshall(s);
        if (s->status == 0) wmove(s->wcmd, s->cmdy, s->cmdx);
        else                wmove(s->wedit, s->edity, s->editx);
        processkey(s, getch());
        if ((s->status < 2) && *s->msg) {
            wscrl(s->winfo, -1);
            mvwprintw(s->winfo, 0, 0, "%-80s", s->msg);
            s->msg[0] = 0;
        }
    } while (s->status != 2);
    saveblock(s);
    windowsdestroy(s);

#if 0
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
#endif

    // end ncurses
    move(25, 0);
    endwin();

    if (*s->msg) printf("%s\n", s->msg);
    return 0;
}
