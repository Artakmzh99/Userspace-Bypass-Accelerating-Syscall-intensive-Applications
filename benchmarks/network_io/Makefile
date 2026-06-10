CC=gcc
CFLAGS=-O2 -Wall -Wextra

all: network_io_bench

network_io_bench: network_io_bench.c
	$(CC) $(CFLAGS) network_io_bench.c -o network_io_bench

run: network_io_bench
	./network_io_bench 67108864

clean:
	rm -f network_io_bench
