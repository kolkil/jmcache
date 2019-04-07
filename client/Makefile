CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11

all: main clean

debug: CFLAGS += -DDEBUG
debug: all

main: console_client.c
	$(CC) $(CFLAGS) console_client.c -o console_client.out

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch;
