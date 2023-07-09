CC = gcc
CFLAGS = -lncurses -lm
SOURCES = ./src/*.c

all: build run clean

build:
	$(CC) $(SOURCES) $(CFLAGS) -o abyss

run:
	./abyss

clean:
	rm abyss
