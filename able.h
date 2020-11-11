#ifndef ABLE_H_INCLUDED
#define ABLE_H_INCLUDED

#include <ncurses.h>

struct ableInfo {
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
};

void refreshall(struct ableInfo *s);
void windowscreate(struct ableInfo *s);
void windowsdestroy(struct ableInfo *s);
void loadsource(struct ableInfo *s);
void saveblock(struct ableInfo *s);
void processkey(struct ableInfo *s, int ch);

#if 0
int addframe(struct ableInfo *s);
void freescreens(struct ableInfo *s);
void refresh_curpage(struct ableInfo *s);
int edit(struct ableInfo *s);
int docmd(struct ableInfo *s, const char *cmd);
int updatepageno(struct ableInfo *s);
#endif

#endif
