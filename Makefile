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

all: fabled

fabled: main.o fabled.o utils.o
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) $(LFLAGS) \
	-o fabled \
	main.o fabled.o utils.o \
	$(LFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o main.o -c main.c

fabled.o: fabled.c fabled.h
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o fabled.o -c fabled.c

utils.o: utils.c utils.h fabled.h
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o utils.o -c utils.c

clean:
	rm *.o
