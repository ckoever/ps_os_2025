.PHONY: all clean

CFLAGS = -std=c11 -Wall -Wextra

all: main

main: task3.o
	$(CC) $(CFLAGS) -o $@ $^

task3.o: task3.c
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm -rf *.o key.c main
