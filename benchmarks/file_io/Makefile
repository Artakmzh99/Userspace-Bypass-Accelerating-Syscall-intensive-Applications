CC=gcc
CFLAGS=-O2 -Wall -Wextra

all: file_io_bench

file_io_bench: file_io_bench.c
	$(CC) $(CFLAGS) file_io_bench.c -o file_io_bench

run: file_io_bench
	./file_io_bench 67108864

clean:
	rm -f file_io_bench
