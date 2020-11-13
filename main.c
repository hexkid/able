#include "able.h"

int main(int argc, char **argv) {
    struct ableInfo s[1] = { 0 };
    setfname(s, (argc > 1) ? argv[1] : "blocks.fb"); // default name

    startcurses();
    loadsource(s);
    windowscreate(s);
    repl(s);
    windowsdestroy(s);
    endcurses();

    return 0;
}
