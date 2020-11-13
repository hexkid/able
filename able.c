#include <ctype.h>
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "able.h"

static void newblock(struct ableInfo *s);
static void newpage(struct ableInfo *s);
static void devicetomemory(struct ableInfo *s);
static void addscreen(struct ableInfo *s);

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
        devicetomemory(s);
        if (s->current++ == s->ns) addscreen(s);
        return;
    }
    if (ch == KEY_PPAGE) {
        if (s->current > 0) { devicetomemory(s); s->current--; }
        else flash();
    }
    if (ch == 'Q') {
        s->status = 2;
    }
    sprintf(s->msg, "got key %08x", ch);
    return;
}

void refreshall(struct ableInfo *s) {
    wnoutrefresh(stdscr);
    wnoutrefresh(s->wpage);
    wnoutrefresh(s->wsource);
    wnoutrefresh(s->wstatus);
    wnoutrefresh(s->winfo);
    if (s->status == 0) {
        wnoutrefresh(s->wedit);
        wnoutrefresh(s->wcmd);
        wmove(s->wcmd, s->cmdy, s->cmdx);
    }
    if (s->status == 1) {
        wnoutrefresh(s->wcmd);
        wnoutrefresh(s->wedit);
        wmove(s->wedit, s->edity, s->editx);
    }
    doupdate();
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

    s->wpage = newwin(1, 20, 2, 3);
    wbkgd(s->wpage, COLOR_PAIR(1));
    mvwprintw(s->wpage, 0, 0, " screen #%d (%d/%d) ", s->current + 1, s->current, s->ms);

    s->wsource = newwin(1, 11, 2, 58);
    wbkgd(s->wsource, COLOR_PAIR(1));
    mvwprintw(s->wsource, 0, 0, "%11s", s->srcname);

    s->wedit = newwin(16, 64, 4, 4);
    keypad(s->wedit, TRUE);
    wbkgd(s->wedit, COLOR_PAIR(10));
    for (int k = 0; k < 16; k++) {
        mvwprintw(s->wedit, k, 0, "%.64s", s->s[s->current] + 1024*k);
    }

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
    mvaddch(21, 41, '<');
    //mvwprintw(s->wcmd, 0, 0, "quit");

    s->wstatus = newwin(1, 20, 21, 49);
    wbkgd(s->wstatus, COLOR_PAIR(3));
    //mvwprintw(s->wstatus, 0, 0, "wstatus 901234567890");

    s->winfo = newwin(4, 80, 22, 0);
    idlok(s->winfo, TRUE);
    scrollok(s->winfo, TRUE);
    wbkgd(s->winfo, COLOR_PAIR(23));

    refresh();
}

void windowsdestroy(struct ableInfo *s) {
    delwin(s->wpage); s->wpage = NULL;
    delwin(s->wsource); s->wsource = NULL;
    delwin(s->wedit); s->wedit = NULL;
    delwin(s->wcmd); s->wcmd = NULL;
    delwin(s->wstatus); s->wstatus = NULL;
    delwin(s->winfo); s->winfo = NULL;
}

void loadsource(struct ableInfo *s) {
    s->current = 0;
    FILE *f = fopen(s->srcname, "rb");
    if (f) {
#if 0
        memset(s->s, ' ', s->ms * sizeof *(s->s));
        if (s->ms < 2) {
            void *tmp = realloc(s->s, 2 * sizeof *(s->s));
            if (!tmp) {
                strcpy(s->msg, "Not enough memory");
                s->status = 2;
                return 1;
            }
            s->s = tmp;
            s->ms = s->ns = 2;
            memset(s->s, ' ', 2 * sizeof *(s->s));
        }
        return 1;
#endif
        s->ns = 0;
        for (;;) {
            char tmp[1024];
            memset(tmp, ' ', 1024);
            errno = 0;
            int n = fread(tmp, 1, 1024, f);
            if ((n > 0) && (errno == 0)) {
                if (s->ns == s->ms) {
                    void *stmp = realloc(s->s, ++s->ms * sizeof *(s->s));
                    if (!stmp) {
                        strcpy(s->msg, "Not enough memory");
                        s->status = 2;
                        fclose(f);
                        return;
                    }
                    s->s = stmp;
                }
                for (int k = 0; k < 1024; k++) {
                    if (tmp[k] < 32) tmp[k] = ' ';
                    if (tmp[k] > 0x7e) tmp[k] = ' ';
                }
                memcpy(s->s[s->ns++], tmp, 1024);
            } else {
                if (n > 0) {
                    memcpy(s->s[s->ns], tmp, n);
                    sprintf(s->msg, "Error: %s\n", strerror(errno));
                } else {
                    if (errno) {
                        sprintf(s->msg, "Error: %s\n", strerror(errno));
                        fclose(f);
                        return;
                    } else {
                        fclose(f);
                        sprintf(s->msg, "read %d screens", s->ns);
                        return;
                    }
                }
                s->status = 2;
                fclose(f);
                return;
            }
        }
    } else {
        newblock(s);
    }
}

void saveblock(struct ableInfo *s) {
    if (s) return;
    return;
}

#if 0
void freescreens(struct ableInfo *s) {
    free(s->s);
    s->s = NULL;
    s->ns = 0;
    s->current = 0;
}

void refresh_curpage(struct ableInfo *s) {
    updatepageno(s);
    for (int k = 0; k < 16; k++) {
        mvprintw(5+k, 4, "%.64s", s->s[s->current] + 64*k);
    }
}

int addpage(struct ableInfo *s) {
    void *tmp = realloc(s->s, (s->ns + 1) * sizeof *(s->s));
    if (tmp == NULL) {
        strcpy(s->msg, "Not enough memory");
        s->status = 0;
        return 1;
    }
    s->s = tmp;
    memset(s->s[s->ns], ' ', sizeof *(s->s));
    char stmp[65];
    time_t tt = time(0);
    struct tm t = *gmtime(&tt);
    sprintf(stmp, "\\ <TITLE>                        "
                  "                    %04d-%02d-%02d",
                  t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    strncpy(s->s[s->ns], stmp, 64);
    s->ns += 1;
    updatepageno(s);
    return 0;
}

int edit(struct ableInfo *s) {
    move(s->edity, s->editx);
    unsigned ch = getch();
    mvprintw(24, 0, "key: 0x%04x        ", ch);
    if ((ch == KEY_UP)    && (s->edity > 5))  move(--s->edity, s->editx);
    if ((ch == KEY_DOWN)  && (s->edity < 20)) move(++s->edity, s->editx);
    if ((ch == KEY_LEFT)  && (s->editx > 4))  move(s->edity, --s->editx);
    if ((ch == KEY_RIGHT) && (s->editx < 67)) move(s->edity, ++s->editx);
    if ((ch == KEY_BACKSPACE) && (s->editx > 4)) {
        mvaddch(s->edity, --s->editx, ' ');
        move(s->edity, s->editx);
    }
    if (ch == KEY_NPAGE) {
        s->current++;
        if (s->current == s->ns) addpage(s);
        refresh_curpage(s);
    }
    if (ch == KEY_PPAGE) {
        if (s->current) s->current--;
        refresh_curpage(s);
    }
    if ((ch >> 7) != 0) return 0; // ignore other 'special' keys
    if (ch == '\t') {
        s->status = 1;
    }
    if ((ch == '\n') || (ch == '\r')) {
        s->editx = 4;
        s->edity++;
        if (s->edity == 21) s->edity = 5;
    }
    if (isgraph((unsigned char)ch) || (ch == ' ')) {
        mvaddch(s->edity, s->editx++, ch);
        if (s->editx == 68) {
            s->editx = 4;
            s->edity++;
            if (s->edity == 21) s->edity = 5;
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
        fseek(f, 1024 * s->current, SEEK_SET);
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

int updatepageno(struct ableInfo *s) {
    mvprintw(3, 4, "Page %d/%d  ", s->current + 1, s->ns ? s->ns : 1);
    return 0;
}

int addframe(struct ableInfo *s) {
    mvprintw(0, 4, "Welcome to able - (A)nother (BL)ock (E)ditor for Forth fans");
    updatepageno(s);
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
        s->s = realloc(s->s, 2 * sizeof *(s->s)); // have faith, using few Ks
        s->ms = 2;
    }
    memset(s->s, ' ', s->ms * sizeof *(s->s));
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
        s->s = realloc(s->s, (++s->ms) * sizeof *(s->s)); // have faith, using few Ks
    }
    char title[65];
    time_t tt = time(0);
    struct tm t = *gmtime(&tt);
    sprintf(title, "( <TITLE> )                       "
                   "                    %04d-%02d-%02d",
                   t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
    strncpy(s->s[s->ns], title, 64); // don't mind the absence of '\0'
    memset(s->s[s->ns] + 64, ' ', 960);
    sprintf(s->msg, "screen #%d added to block.", s->ns++);
}

void devicetomemory(struct ableInfo *s) {
    chtype lin[64];
    char lin8[64];
    for (int k = 0; k < 16; k++) {
        move(4 + k, 4);
        inchnstr(lin, 64);
        for (int kk = 0; kk < 64; kk++) lin8[kk] = (lin[kk] & 0x7f);
        memmove(s->s[s->current] + 64*k, lin8, 64);
    }
}

void addscreen(struct ableInfo *s) {
    if (s->ns == s->ms) {
        s->s = realloc(s->s, ++s->ms * sizeof *(s->s));
    }
    memset(s->s[s->ns++], ' ', 1024);
}
