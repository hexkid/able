CC = /usr/bin/gcc
CFLAGS = -std=c11 -pedantic
WFLAGS = -Wall -Wextra -Wno-missing-braces -Wno-missing-field-initializers \
         -Wno-empty-body \
         -Wformat=2 -Wswitch-default -Wswitch-enum -Wcast-align \
         -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 \
         -Wstrict-prototypes -Winline -Wundef -Wnested-externs -Wcast-qual \
         -Wshadow -Wunreachable-code -Wfloat-equal -Wstrict-aliasing=2 \
         -Wredundant-decls -Wold-style-definition -Werror
OFLAGS = -O3 -DNDEBUG
#OFLAGS = -O0 -ggdb
FFLAGS = -fno-omit-frame-pointer -fno-common -fstrict-aliasing
LFLAGS = -lncurses

all: able

able: main.o able.o utils.o
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) $(LFLAGS) \
	-o able \
	main.o able.o utils.o \
	$(LFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o main.o -c main.c

able.o: able.c able.h
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o able.o -c able.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o utils.o -c utils.c

clean:
	rm *.o
