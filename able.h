#ifndef ABLE_H_INCLUDED
#define ABLE_H_INCLUDED

#include <ncurses.h>

struct ableInfo {
    unsigned status;         // 0: command -- 1: edit -- 2: quit
    char srcname[81];        // name of blocks file on disk
    char (*s)[16][64];       // screens, not strings: no '\0'
    unsigned ms, ns, cs;     // number of screens: allocated, used, current
    unsigned edtx, edty;     // (x, y) for edit area
    unsigned cmdx, cmdy;     // (x, y) for commad prompt (y is constant)
    WINDOW *wpge, *wsrc, *wedt, *wcmd, *wstt, *winf;
};

void setfname(struct ableInfo *s, const char *fname);
void startcurses(void);
void loadsource(struct ableInfo *s);
void windowscreate(struct ableInfo *s);

void rep(struct ableInfo *s);

void windowsdestroy(struct ableInfo *s);
void endcurses(void);

#endif
