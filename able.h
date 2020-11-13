#ifndef ABLE_H_INCLUDED
#define ABLE_H_INCLUDED

#include <ncurses.h>

struct ableInfo {
    int major, minor, patch; // semantic versioning
    unsigned status;         // 0: command -- 1: edit -- 2: quit
    char srcname[81];        // name of blocks file on disk
    char (*s)[16][64];       // screens, not strings: no '\0'
    unsigned ms, ns, cs;     // number of screens: allocated, used, current
    unsigned edtx, edty;     // (x, y) for edit area
    unsigned cmdx, cmdy;     // (x, y) for commad prompt (y is constant)
    WINDOW *wpage, *wsource, *wedit;
    WINDOW *wcmd, *wstatus, *winfo;
};

void startcurses(void);
void setfname(struct ableInfo *s, const char *fname);
void loadsource(struct ableInfo *s);
void windowscreate(struct ableInfo *s);

void refreshall(struct ableInfo *s);
void rep(struct ableInfo *s);
void processkey(struct ableInfo *s, int ch);
void addscreen(struct ableInfo *s);

void windowsdestroy(struct ableInfo *s);
void endcurses(void);

#if 0
int addframe(struct ableInfo *s);
void freescreens(struct ableInfo *s);
void refresh_curpage(struct ableInfo *s);
int edit(struct ableInfo *s);
int docmd(struct ableInfo *s, const char *cmd);
int updatepageno(struct ableInfo *s);
#endif

#endif
