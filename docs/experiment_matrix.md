# Experiment Matrix

This document tracks the reproduction status of the paper experiments.

## Implemented Baseline Experiments

| ID | Experiment | Goal | Implementation | Result | Status |
|---|---|---|---|---|---|
| E1 | Syscall overhead | Measure average cost of basic syscall-related operations | benchmarks/syscall_overhead | results/raw/syscall_overhead.csv | Done |
| E2 | File I/O syscall intensity | Compare small writes vs larger writes | benchmarks/file_io | results/raw/file_io.csv | Done |
| E3 | Network I/O syscall intensity | Compare small sends vs larger sends | benchmarks/network_io | results/raw/network_io.csv | Done |

## Paper Main Experiment Blocks

| Paper block | Paper goal | Our reproduction plan | Result file | Status |
|---|---|---|---|---|
| Block 1: I/O Micro-benchmark | Reproduce Figure 5: READ syscall throughput for 64B, 256B, 1KiB, 4KiB buffers; optionally compare with io_uring | Implemented ARM64 baseline READ IOPS benchmark over /dev/shm; UB comparison pending x86_64 artifact results | results/raw/paper_io_micro_baseline.csv | Baseline done |
| Block 2: Redis | Reproduce Figure 7: Redis GET/SET RPS for different data sizes | Install Redis, run redis-benchmark for GET/SET with data sizes 1B, 64B, 256B, 1KiB, 4KiB, 16KiB | results/raw/paper_redis.csv | Not started |
| Block 3: Nginx | Reproduce Figure 8 and Figure 9: Nginx RPS vs file size and worker count | Install Nginx and wrk; serve static files of 1KB, 4KB, 16KB, 64KB, 256KB; measure RPS | results/raw/paper_nginx.csv | Not started |
| Block 4: Raw Socket vs eBPF | Reproduce Figure 10: UDP packet processing throughput for raw socket and optionally eBPF/XDP | Implement raw UDP packet receiver and sender; measure PPS for 128B, 512B, 1472B packets | results/raw/paper_raw_socket.csv | Not started |