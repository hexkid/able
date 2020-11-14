#include "able.h"

int main(int argc, char **argv) {
    struct ableInfo *s = newinfo((argc > 1) ? argv[1] : "blocks.fb");
    startcurses(s);
    repl(s);
    endcurses(s);
    delinfo(s);
    return 0;
}
