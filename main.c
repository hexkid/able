#include "able.h"

int main(int argc, char **argv) {
    struct ableInfo *s = newinfo((argc > 1) ? argv[1] : "blocks.fb");
    startcurses();
    windowscreate(s);
    repl(s);
    windowsdestroy(s);
    endcurses();
    delinfo(s);
    return 0;
}
