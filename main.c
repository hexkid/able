#include "able.h"

int main(int argc, char **argv) {
    struct ableInfo s[1] = { 0 };
    setfname(s, (argc > 1) ? argv[1] : "blocks.fb"); // default name

    startcurses();
    loadsource(s);
    windowscreate(s);
    // REPL: loop part here
    do {
        rep(s);
    } while (s->status != 2);
    windowsdestroy(s);
    endcurses();

    return 0;
}
