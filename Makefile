CC = gcc

CFLAGS = -Wall -Wextra -pedantic
CFLAGS += $(shell pkg-config --cflags sdl3)
CFLAGS += $(shell pkg-config --cflags sdl3-ttf)

LDFLAGS = -lSDL3_ttf -lSDL3 -lm

TARGETS = vis

.DEFAULT_GOAL := all

all: vis

vis: cvisualiser.o loadData.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

cvisualiser.o: cvisualiser.c
	$(CC) $(CFLAGS) -c $< -o $@

loadData.o: loadData.c loadData.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o

debug: CFLAGS += -g
debug: all
