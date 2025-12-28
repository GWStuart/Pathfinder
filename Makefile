CC = gcc

CFLAGS = -Wall -Wextra -pedantic -O0
CFLAGS += $(shell pkg-config --cflags sdl3 sdl3-ttf)

LDFLAGS = $(shell pkg-config --libs sdl3 sdl3-ttf) -lm

TARGET = vis
OBJS = cvisualiser.o loadData.o renderUtils.o config.o stringUtils.o printUtils.o

.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

debug: CFLAGS += -g -fsanitize=address
debug: clean all

