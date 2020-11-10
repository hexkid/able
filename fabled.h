#ifndef FABLED_H_INCLUDED
#define FABLED_H_INCLUDED

struct fabledStatus {
    unsigned status;       // 0: quit -- 1: command -- 2: editing
    unsigned x, y;         // (x, y) for edit area
    char srcname[81];      // name of blocks file on disk
    char (*s)[1024];       // screens (1024 chars each)
    unsigned ns;           // number of screens
    unsigned current;      // current screen
    char msg[80];          // message
};


int addframe(struct fabledStatus *s);
int loadsource(struct fabledStatus *s);
void freescreens(struct fabledStatus *s);
void refresh_curpage(struct fabledStatus *s);
int edit(struct fabledStatus *s);
int docmd(struct fabledStatus *s, const char *cmd);

#endif
