#include <ctype.h>
#include <stdio.h>

int main(void) {
    FILE *f = fopen("count.inc", "r+");
    char line[100];
    fgets(line, sizeof line, f);
    int val = 0;
    char *p = line;
    while (*p) {
        if (isdigit((unsigned char)*p)) {
            val *= 10;
            val += *p - '0';
        }
        p++;
    }
    rewind(f);
    fprintf(f, "int count = %d;\n", val + 1);
    fclose(f);
    return 0;
}
