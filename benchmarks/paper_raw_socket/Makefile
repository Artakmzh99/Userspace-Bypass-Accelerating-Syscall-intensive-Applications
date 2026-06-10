CC=gcc
CFLAGS=-O2 -Wall -Wextra

all: udp_pps_bench

udp_pps_bench: udp_pps_bench.c
	$(CC) $(CFLAGS) udp_pps_bench.c -o udp_pps_bench

run: udp_pps_bench
	./udp_pps_bench 12

clean:
	rm -f udp_pps_bench
