.PHONY: all clean create_key

CFLAGS = -std=c11 -Wall -Wextra

all: create_key main

main: main.o key.o
	Linking main 
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c
	Compiling $^ 
	$(CC) -c $(CFLAGS) -o $@ $^

create_key:
	Creating key.c
	@if [ -f key.c ]; then rm key.c; fi
	envsubst < key.raw > key.c

key.c: create_key

key.o: key.c
	Compiling $^ 
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm -rf *.o key.c main
