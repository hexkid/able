#include <ctype.h>
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "able.h"
#include "utils.h"

void refreshall(struct ableInfo *s) {
    wnoutrefresh(stdscr);
    wnoutrefresh(s->wpage);
    wnoutrefresh(s->wsource);
    wnoutrefresh(s->wedit);
    wnoutrefresh(s->wcmd);
    wnoutrefresh(s->wstatus);
    wnoutrefresh(s->winfo);
    doupdate();
}

void window_setup(struct ableInfo *s) {
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_GREEN);
    init_pair(3, COLOR_WHITE, COLOR_RED);

    s->wpage = newwin(1, 10, 3, 4);
    wbkgd(s->wpage, COLOR_PAIR(2));
    mvwprintw(s->wpage, 0, 0, "wpage 789012");

    s->wsource = newwin(1, 53, 3, 15);
    wbkgd(s->wsource, COLOR_PAIR(3));
    mvwprintw(s->wsource, 0, 0, "wsource 90123456789012345678901234567890123456789012345");

    s->wedit = newwin(16, 64, 5, 4);
    wbkgd(s->wedit, COLOR_PAIR(1));
    mvwprintw(s->wedit, 0, 0, "wedit 789012345678901234567890123456789012345678901234567890123456");

    s->wcmd = newwin(1, 24, 22, 6);
    wbkgd(s->wcmd, COLOR_PAIR(2));
    mvwprintw(s->wcmd, 0, 0, "wcmd 678901234567890123456");

    s->wstatus = newwin(1, 40, 22, 31);
    wbkgd(s->wstatus, COLOR_PAIR(3));
    mvwprintw(s->wstatus, 0, 0, "wstatus 9012345678901234567890123456789012");

    s->winfo = newwin(3, 80, 23, 0);
    wbkgd(s->winfo, COLOR_PAIR(1));
    mvwprintw(s->winfo, 0, 0, "++++++++\n-----\n::::::\nzzzzzzz");

    refresh();
}

void window_destroy(struct ableInfo *s) {
    delwin(s->wpage); s->wpage = NULL;
    delwin(s->wsource); s->wsource = NULL;
    delwin(s->wedit); s->wedit = NULL;
    delwin(s->wcmd); s->wcmd = NULL;
    delwin(s->wstatus); s->wstatus = NULL;
    delwin(s->winfo); s->winfo = NULL;
}

#if 0
int loadsource(struct ableInfo *s) {
    FILE *f = fopen(s->srcname, "rb");
    s->current = 0;
    if (!f) {
        s->ns = 2;
        s->s = malloc(2 * sizeof *(s->s));
        if (!s->s) {
            strcpy(s->msg, "Not enough memory");
            s->status = 0;
        } else {
            memset(s->s, ' ', 2 * sizeof *(s->s));
            char tmp[65];
            time_t tt = time(0);
            struct tm t = *gmtime(&tt);
            sprintf(tmp, "\\ <TITLE>                        "
                         "                    %04d-%02d-%02d",
                         t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
            strncpy(s->s[0], tmp, 64);
            strncpy(s->s[1], tmp, 64);
        }
        updatepageno(s);
        return 1;
    } else {
        for (;;) {
            char tmp[1024];
            memset(tmp, ' ', 1024);
            errno = 0;
            int n = fread(tmp, 1, 1024, f);
            if ((n > 0) && (errno == 0)) {
                void *stmp = realloc(s->s, ++s->ns * sizeof *(s->s));
                if (stmp == NULL) {
                    strcpy(s->msg, "Not enough memory");
                    s->status = 0;
                }
                s->s = stmp;
                for (int k = 0; k < 1024; k++) {
                    if (tmp[k] < 32) tmp[k] = ' ';
                    if (tmp[k] > 0x7e) tmp[k] = ' ';
                }
                memcpy(s->s[s->ns - 1], tmp, 1024);
                updatepageno(s);
            } else {
                break;
            }
            if (n < 1024) break;
        }
    }
    refresh_curpage(s);
    fclose(f);
    return 0;
}

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
        if (!f) able_quit(strerror(errno));
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
