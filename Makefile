CC=gcc
CFLAGS=-Wall -Wextra -pedantic
DEBUG_FLAGS=-g -DDEBUG=1
LIBS += -lm

.PHONY: all clean debug test

all: build

build: CFLAGS += -O
build: main.c
	$(CC) -lm $(CFLAGS) -o jaya main.c $(LIBS)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: build

clean:
	rm -f jaya

test:
	./run_test.sh