CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11

all: main clean

debug: CFLAGS += -DDEBUG
debug: all

prof: CFLAGS += -pg
prof: all

main: lib.o console_client simple_insert

console_client: lib.o console_client.c
	$(CC) $(CFLAGS)  mpocket_client.o console_client.c -o console_client.out

simple_insert: lib.o simple_insert.c
	$(CC) $(CFLAGS) mpocket_client.o simple_insert.c -o simple_insert.out

lib.o:
	$(CC) $(CFLAGS) -c ../lib/*

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch; rm -f ../*/*.gch;
