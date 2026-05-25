CC = gcc

CFLAGS = -Wall -Wextra -pedantic -std=c11

SRC = src/main.c \
	src/buffer.c \
	src/io.c \
	src/compression.c \
	src/crypto.c

OUT = build/editor

BENCH = build/benchmark

all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) -lz

benchmark:
	mkdir -p build
	$(CC) $(CFLAGS) \
	src/benchmark_main.c \
	src/buffer.c \
	src/io.c \
	src/compression.c \
	src/crypto.c \
	-o $(BENCH) -lz

run: all
	./$(OUT)

clean:
	rm -rf build