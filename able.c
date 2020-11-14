#include <ctype.h>
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "able.h"
#include "VERSION.h" // define (and document) semantic versioning

struct ableInfo {
    unsigned status;         // 0: command -- 1: edit -- 2: quit
    char srcname[81];        // name of blocks file on disk
    char (*s)[16][64];       // screens, not strings: no '\0'
    unsigned ms, ns, cs;     // number of screens: allocated, used, current
    unsigned edtx, edty;     // (x, y) for edit area
    unsigned cmdx, cmdy;     // (x, y) for commad prompt (y is constant)
    WINDOW *wpge, *wsrc, *wedt, *wcmd, *wstt, *winf;
};

static void addmessage(struct ableInfo *s, const char *msg, const char *extra);
static void initscreen(struct ableInfo *s, unsigned n);
static void addscreen(struct ableInfo *s);
static void processkey(struct ableInfo *s, int ch);
static void refreshall(struct ableInfo *s);
static void update_wpge(struct ableInfo *s);
static void update_wsrc(struct ableInfo *s);
static void update_wedt(struct ableInfo *s);
static void update_wcmd(struct ableInfo *s);
static void update_wstt(struct ableInfo *s);
static void update_winf(struct ableInfo *s);

void setfname(struct ableInfo *s, const char *fname) {
    strcpy(s->srcname, fname);
}

void startcurses(void) {
    initscr();
    cbreak();
    noecho();
    start_color();
}

void loadsource(struct ableInfo *s) {
    s->cs = 0;
    FILE *f = fopen(s->srcname, "rb");
    if (f) {
        s->ns = 0;
        for (;;) {
            char tmp[1024];
            memset(tmp, ' ', 1024);
            errno = 0;
            int n = fread(tmp, 1, 1024, f);
            if ((n > 0) && (errno == 0)) {
                if (s->ns == s->ms) {
                    s->ms += 1;
                    s->s = realloc(s->s, s->ms * 1024);
                }
                for (int k = 0; k < 1024; k++) {
                    if (tmp[k] < 32) tmp[k] = ' ';
                    if (tmp[k] > 0x7e) tmp[k] = ' ';
                }
                memmove(s->s[s->ns], tmp, 1024);
                s->ns += 1;
            } else {
                if (n > 0) {
                    memmove(s->s[s->ns], tmp, n);
                    s->ns += 1;
                    addmessage(s, "Error:", strerror(errno));
                } else {
                    if (errno) {
                        addmessage(s, "Error:", strerror(errno));
                        fclose(f);
                        return;
                    } else {
                        fclose(f);
                        addmessage(s, "incomplete final page", 0);
                        return;
                    }
                }
                s->status = 2;
                fclose(f);
                return;
            }
        }
    } else {
        initscreen(s, 0);
        initscreen(s, 1);
        s->ns = 2;
        s->cs = 0;
    }
}

void windowscreate(struct ableInfo *s) {
    init_pair( 1, COLOR_WHITE,   COLOR_BLACK);
    init_pair( 2, COLOR_CYAN,    COLOR_BLACK);
    init_pair( 3, COLOR_GREEN,   COLOR_BLACK);
    init_pair( 4, COLOR_YELLOW,  COLOR_BLACK);
    init_pair( 5, COLOR_BLACK,   COLOR_WHITE);
    init_pair( 6, COLOR_MAGENTA, COLOR_WHITE);
    init_pair( 7, COLOR_BLUE,    COLOR_WHITE);
    init_pair( 8, COLOR_RED,     COLOR_WHITE);
    init_pair( 9, COLOR_WHITE,   COLOR_MAGENTA);
    init_pair(10, COLOR_WHITE,   COLOR_BLUE);
    init_pair(11, COLOR_CYAN,    COLOR_BLUE);
    init_pair(12, COLOR_GREEN,   COLOR_BLUE);
    init_pair(13, COLOR_YELLOW,  COLOR_BLUE);
    init_pair(14, COLOR_BLACK,   COLOR_CYAN);
    init_pair(15, COLOR_BLUE,    COLOR_CYAN);
    init_pair(16, COLOR_RED,     COLOR_CYAN);
    init_pair(17, COLOR_BLACK,   COLOR_GREEN);
    init_pair(18, COLOR_BLUE,    COLOR_GREEN);
    init_pair(19, COLOR_BLACK,   COLOR_YELLOW);
    init_pair(20, COLOR_MAGENTA, COLOR_YELLOW);
    init_pair(21, COLOR_BLUE,    COLOR_YELLOW);
    init_pair(22, COLOR_RED,     COLOR_YELLOW);
    init_pair(23, COLOR_WHITE,   COLOR_RED);

    mvprintw(0, 3, "able - (A)nother (BL)ock (E)ditor - version %d.%d.%d",
          VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    s->wpge = newwin(1, 20, 2, 3);
    wbkgd(s->wpge, COLOR_PAIR(1));
    update_wpge(s);

    s->wsrc = newwin(1, 11, 2, 58);
    wbkgd(s->wsrc, COLOR_PAIR(1));
    update_wsrc(s);

    s->wedt = newwin(16, 64, 4, 4);
    keypad(s->wedt, TRUE);
    wbkgd(s->wedt, COLOR_PAIR(10));
    update_wedt(s);

    mvaddch(3, 3, '+');
    mvhline(3, 4, '-', 64);
    mvaddch(3, 68, '+');
    for (int k = 0; k < 16; k++) mvprintw(k + 4, 1, "%2d|", k);
    for (int k = 0; k < 16; k++) mvaddch(k + 4, 68, '|');
    mvaddch(20, 3, '+');
    mvhline(20, 4, '-', 64);
    mvaddch(20, 68, '+');

    mvaddch(21, 2, '>');
    s->wcmd = newwin(1, 36, 21, 4);
    keypad(s->wcmd, TRUE);
    wbkgd(s->wcmd, COLOR_PAIR(13));
    update_wcmd(s);
    mvaddch(21, 41, '<');

    s->wstt = newwin(1, 20, 21, 49);
    wbkgd(s->wstt, COLOR_PAIR(3));
    update_wstt(s);

    s->winf = newwin(4, 80, 22, 0);
    idlok(s->winf, TRUE);
    scrollok(s->winf, TRUE);
    wbkgd(s->winf, COLOR_PAIR(23));
    update_winf(s);
}

void repl(struct ableInfo *s) {
    do {
        refreshall(s);
        int ch = wgetch((s->status == 0) ? s->wcmd : s->wedt);
        processkey(s, ch);
    } while (s->status != 2);
}

void windowsdestroy(struct ableInfo *s) {
    delwin(s->wpge); s->wpge = NULL;
    delwin(s->wsrc); s->wsrc = NULL;
    delwin(s->wedt); s->wedt = NULL;
    delwin(s->wcmd); s->wcmd = NULL;
    delwin(s->wstt); s->wstt = NULL;
    delwin(s->winf); s->winf = NULL;
}

void endcurses(void) {
    move(25, 0);
    endwin();
}

#if 0
static void newblock(struct ableInfo *s);
static void newpage(struct ableInfo *s);
static void saveblock(struct ableInfo *s);

void saveblock(struct ableInfo *s) {
    if (s) return;
    return;
}

#if 0
void freescreens(struct ableInfo *s) {
    free(s->s);
    s->s = NULL;
    s->ns = 0;
    s->cs = 0;
}

void refresh_curpage(struct ableInfo *s) {
    for (int k = 0; k < 16; k++) {
        mvprintw(5+k, 4, "%.64s", s->s[s->cs] + 64*k);
    }
}

int addpage(struct ableInfo *s) {
    void *tmp = realloc(s->s, (s->ns + 1) * 1024);
    if (tmp == NULL) {
        addmessage(s, "Not enough memory");
        s->status = 0;
        return 1;
    }
    s->s = tmp;
    memset(s->s[s->ns], ' ', 1024);
    char stmp[65];
    time_t tt = time(0);
    struct tm t = *gmtime(&tt);
    sprintf(stmp, "\\ <TITLE>                        "
                  "                    %04d-%02d-%02d",
                  t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    strncpy(s->s[s->ns], stmp, 64);
    s->ns += 1;
    return 0;
}

int edit(struct ableInfo *s) {
    move(s->edty, s->edtx);
    unsigned ch = getch();
    mvprintw(24, 0, "key: 0x%04x        ", ch);
    if ((ch == KEY_UP)    && (s->edty > 5))  move(--s->edty, s->edtx);
    if ((ch == KEY_DOWN)  && (s->edty < 20)) move(++s->edty, s->edtx);
    if ((ch == KEY_LEFT)  && (s->edtx > 4))  move(s->edty, --s->edtx);
    if ((ch == KEY_RIGHT) && (s->edtx < 67)) move(s->edty, ++s->edtx);
    if ((ch == KEY_BACKSPACE) && (s->edtx > 4)) {
        mvaddch(s->edty, --s->edtx, ' ');
        move(s->edty, s->edtx);
    }
    if (ch == KEY_NPAGE) {
        s->cs += 1;
        if (s->cs == s->ns) addpage(s);
        refresh_curpage(s);
    }
    if (ch == KEY_PPAGE) {
        if (s->cs) s->cs -= 1;
        refresh_curpage(s);
    }
    if ((ch >> 7) != 0) return 0; // ignore other 'special' keys
    if (ch == '\t') {
        s->status = 1;
    }
    if ((ch == '\n') || (ch == '\r')) {
        s->edtx = 4;
        s->edty++;
        if (s->edty == 21) s->edty = 5;
    }
    if (isgraph((unsigned char)ch) || (ch == ' ')) {
        mvaddch(s->edty, s->edtx++, ch);
        if (s->edtx == 68) {
            s->edtx = 4;
            s->edty++;
            if (s->edty == 21) s->edty = 5;
        }
    }
    return 0;
}

int docmd(struct ableInfo *s, const char *cmd) {
    if (tolower((unsigned char)*cmd) == 'q') {
        s->status = 0;
        return 0;
    }
    if (tolower((unsigned char)*cmd) == 'e') {
        mvprintw(22, 6, "Hit <TAB> for command");
        s->status = 2;
        return 0;
    }
    if (tolower((unsigned char)*cmd) == 's') {
        FILE *f = fopen(s->srcname, "w+b");
        if (!f) /* error */;
        fseek(f, 1024 * s->cs, SEEK_SET);
        chtype lin[65];
        char lin8[65];
        for (int k = 0; k < 16; k++) {
            move(5 + k, 4);
            inchnstr(lin, 64);
            for (int kk = 0; kk < 65; kk++) lin8[kk] = (lin[kk] & 0x7f);
            fwrite(lin8, 1, 64, f);
        }
        fclose(f);
        move(22, 6);
        return 0;
    }
    return -1;
}

int addframe(struct ableInfo *s) {
    mvprintw(0, 4, "Welcome to able - (A)nother (BL)ock (E)ditor for Forth fans");
    mvprintw(3, 15, "%53s", s->srcname);
    mvaddch(4, 3, '+');          mvaddch(4, 68, '+');
    mvaddch(21, 3, '+');         mvaddch(21, 68, '+');
    mvhline(4, 4, '-', 64);      mvhline(21, 4, '-', 64);
    mvvline(5, 3, '|', 16);      mvvline(5, 68, '|', 16);
    mvprintw(22, 4, ">");        mvprintw(22, 31, "<");
    for (int k = 0; k < 16; k++) mvprintw(k + 5, 0, "%2d", k);
    move(22, 6);
    return 0;
}
#endif

void newblock(struct ableInfo *s) {
    if (s->ms < 2) {
        s->ms = 2;
        s->s = realloc(s->s, s->ms * 1024); // have faith, using few Ks
    }
    memset(s->s, ' ', s->ms * 1024);
    char title[65];
    time_t tt = time(0);
    struct tm t = *gmtime(&tt);
    sprintf(title, "\\ <TITLE>                        "
                   "                    %04d-%02d-%02d",
                   t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    strncpy(s->s[0], title, 64); // don't mind the absence of '\0'
    newpage(s);
}

void newpage(struct ableInfo *s) {
    if (s->ns == s->ms) {
        s->ms += 1;
        s->s = realloc(s->s, s->ms * 1024); // have faith, using few Ks
    }
    char title[65];
    time_t tt = time(0);
    struct tm t = *gmtime(&tt);
    sprintf(title, "( <TITLE> )                       "
                   "                    %04d-%02d-%02d",
                   t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    strncpy(s->s[s->ns], title, 64); // don't mind the absence of '\0'
    memset(s->s[s->ns] + 64, ' ', 960);
    s->ns += 1;
    addmessage(s, "1 screen added to block.", 0);
}

#endif

void addmessage(struct ableInfo *s, const char *msg, const char *extra) {
    wscrl(s->winf, 1);
    if (extra && *extra) {
        mvwprintw(s->winf, 3, 0, "%s %s", msg, extra);
    } else {
        mvwprintw(s->winf, 3, 0, "%s", msg);
    }
}

void initscreen(struct ableInfo *s, unsigned n) {
    while (n >= s->ms) addscreen(s);
    memset(s->s[n], ' ', 1024);
}

void addscreen(struct ableInfo *s) {
    if (s->ns == s->ms) {
        s->ms += 1;
        s->s = realloc(s->s, s->ms * 1024);
        addmessage(s, "screen added", 0);
    }
    memset(s->s[s->ns], ' ', 1024);
    s->ns += 1;
    addmessage(s, "screen blanked", 0);
    update_wpge(s);
}

void processkey(struct ableInfo *s, int ch) {
#if 0
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
#endif
    if (ch == KEY_NPAGE) {
        if (s->cs + 1 == s->ns) addscreen(s);
        s->cs += 1;
        update_wpge(s);
        return;
    }
    if (ch == KEY_PPAGE) {
        if (s->cs > 0) s->cs -= 1;
        else           flash();
        update_wpge(s);
    }
    if (ch == 'Q') {
        s->status = 2;
    }
    return;
}

void refreshall(struct ableInfo *s) {
    wnoutrefresh(stdscr);
    wnoutrefresh(s->wpge);
    wnoutrefresh(s->wsrc);
    wnoutrefresh(s->wstt);
    wnoutrefresh(s->winf);
    wnoutrefresh(s->wedt);
    wnoutrefresh(s->wcmd);
    switch (s->status) {
        default: addmessage(s, "invalid state", 0); break;
        case 0: wmove(s->wcmd, s->cmdy, s->cmdx); break;
        case 1: wmove(s->wedt, s->edty, s->edtx); break;
    }
    doupdate();
}

void update_wpge(struct ableInfo *s) {
    for (int k = 0; k < 16; k++) {
        mvwprintw(s->wedt, k, 0, "%.64s", s->s[s->cs][k]);
    }
    mvwprintw(s->wpge, 0, 0, " screen #%d (%d/%d)   ", s->cs + 1, s->ns, s->ms);
}

void update_wsrc(struct ableInfo *s) {
    mvwprintw(s->wsrc, 0, 0, "%11s          ", s->srcname);
}

void update_wedt(struct ableInfo *s) {
    for (int k = 0; k < 16; k++) {
        mvwprintw(s->wedt, k, 0, "%.64s", s->s[s->cs][k]);
    }
}

void update_wcmd(struct ableInfo *s) {
    mvwprintw(s->wcmd, 0, 0, " <TAB>        ");
}

void update_wstt(struct ableInfo *s) {
    mvwprintw(s->wstt, 0, 0, " STATUS       ");
}

void update_winf(struct ableInfo *s) {
    mvwprintw(s->winf, 0, 0, " STATUS       ");
}
