$(CC) = gcc -Wall

all: CC += -g
all: netcheck

release: CC += -DNDEBUG
release: netcheck

netcheck: main.o netcheck.o
	$(CC) -o netcheck main.o netcheck.o `pkg-config --libs libcurl`

main.o: main.c
	$(CC) -c main.c

netcheck.o: netcheck.c
	$(CC) -c netcheck.c `pkg-config --cflags libcurl`

clean:
	rm program main.o netcheck.o