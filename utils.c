#include <ncurses.h>
#include <stdlib.h>

void able_quit(const char *msg) {
    move(24, 0);
    endwin();
    fprintf(stderr, "error: %s\n", msg ? msg : "(unknown error)");
    exit(EXIT_FAILURE);
}
