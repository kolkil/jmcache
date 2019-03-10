CC = gcc
CFLAGS = -Wall -Wextra -O3

all: main clean

debug: CFLAGS += -DDEBUG
debug: all

test: tests.o clean

main: main.c
	$(CC) $(CFLAGS) tree.o list.o options.o utils.o main.c -o word-counter.out
tests.o: storage.o
	$(CC) $(CFLAGS) data.o hash_table.o tests/main.c -o tests.out

storage.o:
	$(CC) $(CFLAGS) -c storage/*

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch;