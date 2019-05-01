CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99
HEADERS = mycode.h myoperations.h myutils.h
OBJECTS = main.o mycode.o myoperations.o myutils.o
EXEC = format_text

.PHONY = default build clean

default: build

build: format_text

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

run: build
	./$(EXEC) ${ARGS}

clean:
	-rm -f $(OBJECTS)
	-rm -f $(EXEC)
