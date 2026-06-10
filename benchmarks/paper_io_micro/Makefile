CC=gcc
CFLAGS=-O2 -Wall -Wextra

all: read_iops_bench

read_iops_bench: read_iops_bench.c
	$(CC) $(CFLAGS) read_iops_bench.c -o read_iops_bench

run: read_iops_bench
	./read_iops_bench 2000000

clean:
	rm -f read_iops_bench
