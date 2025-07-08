CC = gcc
CFLAGS = -Wall -Wextra -pedantic
CFLAGS += $(shell pkg-config --cflags sdl3)
LDFLAGS = -lSDL3 -lm

TARGETS = vis

.DEFAULT_GOAL := all

all: vis loadData

vis: cvisualiser.c
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

loadData: loadData.c
	$(CC) $< -o $@

clean:
	rm -f $(TARGETS) *.o

debug: CFLAGS += -g
debug: all
