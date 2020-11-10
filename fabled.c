#include <ctype.h>
#include <errno.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fabled.h"
#include "utils.h"

int loadsource(struct fabledStatus *s) {
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

void freescreens(struct fabledStatus *s) {
    free(s->s);
    s->s = NULL;
    s->ns = 0;
    s->current = 0;
}

void refresh_curpage(struct fabledStatus *s) {
    for (int k = 0; k < 16; k++) {
        mvprintw(5+k, 4, "%.64s", s->s[s->current] + 64*k);
    }
}

int edit(struct fabledStatus *s) {
    move(s->y, s->x);
    unsigned ch = getch();
    mvprintw(24, 0, "key: 0x%04x        ", ch);
    if ((ch == KEY_UP)    && (s->y > 5))  move(--s->y, s->x);
    if ((ch == KEY_DOWN)  && (s->y < 20)) move(++s->y, s->x);
    if ((ch == KEY_LEFT)  && (s->x > 4))  move(s->y, --s->x);
    if ((ch == KEY_RIGHT) && (s->x < 67)) move(s->y, ++s->x);
    if ((ch == KEY_BACKSPACE) && (s->x > 4)) {
        mvaddch(s->y, --s->x, ' ');
        move(s->y, s->x);
    }
    if (ch == KEY_NPAGE) {
        s->current++;
        refresh_curpage(s);
    }
    if (ch == KEY_PPAGE) /*void*/;
    if ((ch >> 7) != 0) return 0; // ignore other 'special' keys
    if (ch == '\t') {
        s->status = 1;
    }
    if ((ch == '\n') || (ch == '\r')) {
        s->x = 4;
        s->y++;
        if (s->y == 21) s->y = 5;
    }
    if (isgraph((unsigned char)ch) || (ch == ' ')) {
        mvaddch(s->y, s->x++, ch);
        if (s->x == 68) {
            s->x = 4;
            s->y++;
            if (s->y == 21) s->y = 5;
        }
    }
    return 0;
}

int docmd(struct fabledStatus *s, const char *cmd) {
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
        if (!f) fabledquit(strerror(errno));
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

int addframe(struct fabledStatus *s) {
    mvprintw(0, 4, "Welcome to fabled - your (FA)ncy (BL)ock (ED)itor for Forth fans");
    mvprintw(3, 4, "Page %d/%d", s->current + 1, s->ns ? s->ns : 1);
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
