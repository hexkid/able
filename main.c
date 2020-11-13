#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include "able.h"

#define VERSION "0.1"

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
    mvprintw(0, 3, "Welcome to ABLE - (A)nother (BL)ock (E)ditor - v%s (%d)",
          VERSION, count);
    loadsource(s);
    windowscreate(s);
    do {
        refreshall(s);
        if (s->status == 0) wmove(s->wcmd, s->cmdy, s->cmdx);
        else                wmove(s->wedit, s->edity, s->editx);
        processkey(s, wgetch((s->status == 0) ? s->wcmd : s->wedit));
        if ((s->status < 2) && *s->msg) {
            wscrl(s->winfo, -1);
            mvwprintw(s->winfo, 0, 0, "%-80s", s->msg);
            s->msg[0] = 0;
        }
    } while (s->status != 2);
    saveblock(s);
    windowsdestroy(s);

    // end ncurses
    move(25, 0);
    endwin();

    if (*s->msg) printf("%s\n", s->msg);
    return 0;
}
