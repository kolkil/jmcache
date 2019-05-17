CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11 -D_POSIX_C_SOURCE

all: main clean

debug: CFLAGS += -DDEBUG
debug: all

prof: CFLAGS += -pg
prof: all

test: tests.o clean

main: storage.o communication.o utils.o program.o logs_dealer.o main.c
	$(CC) $(CFLAGS) static_file.o jobs.o protocol.o debug_print.o program.o vector.o hash_table.o data.o socket.o config.o options.o main.c -o mcache-server.out -lpthread

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

logs_dealer.o:
	$(CC) $(CFLAGS) -c logs_dealer/*

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch;
