CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11

all: main clean

debug: CFLAGS += -DDEBUG
debug: all

prof: CFLAGS += -pg
prof: all

main: console_client simple_insert

console_client: lib.o shared.o console_client.c
	$(CC) $(CFLAGS) mpocket_client.o protocol.o debug_print.o console_client.c -o console_client.out

simple_insert: lib.o shared.o simple_insert.c
	$(CC) $(CFLAGS) mpocket_client.o protocol.o debug_print.o simple_insert.c -o simple_insert.out

lib.o:
	$(CC) $(CFLAGS) -c ../lib/*.c

shared.o:
	$(CC) $(CFLAGS) -c ../../shared/*

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch; rm -f ../*/*.gch; rm -f ../../shared/*.gch;
