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

all: updatecount able

updatecount: updatecount.c
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o updatecount updatecount.c

able: main.o able.o
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) $(LFLAGS) \
	-o able \
	main.o able.o \
	$(LFLAGS)
	./updatecount

main.o: main.c
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o main.o -c main.c

able.o: able.c able.h
	$(CC) $(CFLAGS) $(WFLAGS) $(OFLAGS) $(FFLAGS) \
	-o able.o -c able.c

clean:
	rm *.o
