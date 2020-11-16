#ifndef ABLE_H_INCLUDED
#define ABLE_H_INCLUDED

struct ableInfo;                         // defined in able.c

struct ableInfo *newinfo(const char *fname);
void startcurses(struct ableInfo *s);
void repl(struct ableInfo *s);
void endcurses(struct ableInfo *s);
void delinfo(struct ableInfo *s);

#endif
