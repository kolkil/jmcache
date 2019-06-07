CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11 -D_POSIX_C_SOURCE

all: main clean

debug: CFLAGS += -DDEBUG
debug: all

prof: CFLAGS += -pg
prof: all

test: tests.o clean

main: storage.o communication.o utils.o program.o logger.o threading.o shared.o main.c
	$(CC) $(CFLAGS) threading.o queue.o logger.o jobs.o debug_print.o program.o vector.o protocol.o hash_table.o data.o socket.o config.o options.o main.c -o mpocket-server.out -lpthread

threading.o:
	$(CC) $(CFLAGS) -c threading.c threading.h

tests.o: storage.o
	$(CC) $(CFLAGS) data.o hash_table.o tests/main.c -o tests.out

storage.o:
	$(CC) $(CFLAGS) -c storage/*

communication.o:
	$(CC) $(CFLAGS) -c communication/*

utils.o:
	$(CC) $(CFLAGS) -c utils/*

program.o:
	$(CC) $(CFLAGS) -c program.*

logger.o:
	$(CC) $(CFLAGS) -c logger/*

shared.o:
	$(CC) $(CFLAGS) -c ../shared/*

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch; rm -f ../shared/*.gch;
