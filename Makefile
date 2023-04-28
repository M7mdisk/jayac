CC=gcc
CFLAGS=-Wall -Wextra -pedantic
DEBUG_FLAGS=-g -pg -DDEBUG=1
LIBS = -lm 
LIBS_MT = -lpthread
OBJS= utils.o 
OUT=jaya
CMD=100 100 1000000 sphere

.PHONY: all clean build debug test multi multi_debug

all: build

build: CFLAGS += -O
build: main.c utils.h utils.o
	$(CC) $(CFLAGS) -o $(OUT) $(OBJS) main.c $(LIBS)

multi: CFLAGS += -O
multi: multi.c utils.h utils.o
	$(CC) $(CFLAGS) -o $(OUT)_multi $(OBJS) multi.c $(LIBS) $(LIBST_MT)

multi_debug: CFLAGS+= $(DEBUG_FLAGS)
multi_debug: main.c utils.o
	$(CC) $(CFLAGS) -o $(OUT)_multi $(OBJS) multi.c $(LIBS) $(LIBST_MT)

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o $(LIBS)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: main.c utils.o
	$(CC) $(CFLAGS) -o $(OUT) $(OBJS) main.c $(LIBS)

profile: debug
	./$(OUT) $(CMD)
	gprof ./$(OUT) gmon.out


clean:
	rm -f $(OUT) $(OBJS) gmon.out $(OUT)_multi

test:
	./run_test.sh