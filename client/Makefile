CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11

all: main clean

debug: CFLAGS += -DDEBUG
debug: all

prof: CFLAGS += -pg
prof: all

main: lib.o console_client.c simple_insert
	$(CC) $(CFLAGS)  mcache_client.o console_client.c -o console_client.out

simple_insert: lib.o simple_insert.c
	$(CC) $(CFLAGS) mcache_client.o simple_insert.c -o simple_insert.out

lib.o:
	$(CC) $(CFLAGS) -c lib/*

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch;
