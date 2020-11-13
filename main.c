#include "able.h"

int main(int argc, char **argv) {
    struct ableInfo s[1] = { 0 };
    s->major = 0;
    s->minor = 2;
    s->patch = 2;
    setfname(s, (argc > 1) ? argv[1] : "blocks.fb");

    startcurses();
    loadsource(s);
    windowscreate(s);
    do {
        refreshall(s);
        rep(s);            // Read Eval Process... Loop is here
    } while (s->status != 2);
    windowsdestroy(s);
    endcurses();

    return 0;
}
