CC=gcc
CFLAGS=-O2 -Wall -Wextra

all: syscall_bench

syscall_bench: syscall_bench.c
	$(CC) $(CFLAGS) syscall_bench.c -o syscall_bench

run: syscall_bench
	./syscall_bench 1000000

clean:
	rm -f syscall_bench
