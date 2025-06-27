CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -lSDL3 -lm -lcjson

.DEFAULT_GOAL := all
TARGETS = loadData

#pathfinder:
#	$(CC) pathfinder.c $(shell pkg-config --cflags --libs sdl3) -lm -o $@

all: loadData
#pathfinder

#pathfinder: pathfinder.c

loadData: loadData.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(TARGETS) *.o

debug: CFLAGS += -g
debug: all
