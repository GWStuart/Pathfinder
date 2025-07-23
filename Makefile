CC = gcc

CFLAGS = -Wall -Wextra -pedantic
CFLAGS += $(shell pkg-config --cflags sdl3)
CFLAGS += $(shell pkg-config --cflags sdl3-ttf)

LDFLAGS = -lSDL3_ttf -lSDL3 -lm

TARGETS = vis

.DEFAULT_GOAL := all

OBJS = cvisualiser.o loadData.o renderUtils.o config.o

all: vis

vis: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o

debug: CFLAGS += -g
debug: all
