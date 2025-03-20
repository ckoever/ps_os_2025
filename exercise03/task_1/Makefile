CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

.PHONY: all
all: signal_handler

signal_handler: signal_handler.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean: 
	$(RM) signal_handler