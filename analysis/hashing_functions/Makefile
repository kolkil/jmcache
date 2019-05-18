CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11 -D_POSIX_C_SOURCE

all: main clean

main: main.c
	$(CC) $(CFLAGS) main.c -o main.out

clean:
	rm -f *.o; rm -f *.gch; rm -f ./*/*.gch;
