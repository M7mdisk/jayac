CC=gcc
CFLAGS=-Wall -Wextra -pedantic
DEBUG_FLAGS=-g -pg -DDEBUG=1
LIBS = -lm 
OBJS= utils.o 
OUT=jaya
CMD=100 100 1000000 sphere

.PHONY: all clean build debug test

all: build

build: CFLAGS += -O
build: main.c utils.h utils.o
	$(CC) $(CFLAGS) -o $(OUT) $(OBJS) main.c $(LIBS)

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o $(LIBS)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: build

profile: debug
	./$(OUT) $(CMD)
	gprof ./$(OUT) gmon.out


clean:
	rm -f $(OUT) $(OBJS) gmon.out

test:
	./run_test.sh