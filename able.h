#ifndef ABLE_H_INCLUDED
#define ABLE_H_INCLUDED

struct ableInfo;                                    // defined in able.c

struct ableInfo *newinfo(const char *fname);

void startcurses(void);
void windowscreate(struct ableInfo *s);

void repl(struct ableInfo *s);

void windowsdestroy(struct ableInfo *s);
void endcurses(void);

#endif
