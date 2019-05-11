CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11

all: main clean

debug: CFLAGS += -DDEBUG
debug: all

prof: CFLAGS += -pg
prof: all

main: lib.o

lib.o:
	$(CC) $(CFLAGS) -c lib/*

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch;
