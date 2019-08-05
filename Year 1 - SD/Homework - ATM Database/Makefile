CC = gcc
CFLAGS = -Wall -Wextra -std=c99
HEADERS = functions_lib.h
OBJECTS = main.o utils.o client_operations.o admin_operations.o
EXEC = atm

.PHONY = default build clean

default: run

build: atm

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

run: build
	./$(EXEC)

clean:
	rm -f $(OBJECTS)
	rm -f $(EXEC)
	rm -f input.in output.out
