#ifndef ABLE_H_INCLUDED
#define ABLE_H_INCLUDED

struct ableInfo {
    unsigned status;       // 0: quit -- 1: command -- 2: editing
    unsigned x, y;         // (x, y) for edit area
    char srcname[81];      // name of blocks file on disk
    char (*s)[1024];       // screens (1024 chars each)
    unsigned ns;           // number of screens
    unsigned current;      // current screen
    char msg[80];          // message
};


int addframe(struct ableInfo *s);
int loadsource(struct ableInfo *s);
void freescreens(struct ableInfo *s);
void refresh_curpage(struct ableInfo *s);
int edit(struct ableInfo *s);
int docmd(struct ableInfo *s, const char *cmd);
int updatepageno(struct ableInfo *s);

#endif
