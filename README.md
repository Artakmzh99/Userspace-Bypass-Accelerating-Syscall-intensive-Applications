# Userspace-Bypass-Accelerating-Syscall-intensive-Applications
Tesr
# Userspace Bypass: Reproducing Syscall-Intensive Application Experiments

This repository contains a reproduction effort for experiments related to syscall-intensive applications and userspace bypass techniques.

## Environment

The experiments are currently executed inside an Ubuntu virtual machine.

Current setup:

- OS: Ubuntu 26.04
- Architecture: ARM64 / aarch64
- Compiler: GCC
- VM: VirtualBox on macOS

## Experiment 1: Syscall Overhead Benchmark

This experiment measures the average cost of several syscall-related operations:

- empty loop
- getpid()
- read 1 byte from /dev/zero
- write 1 byte to /dev/null

## Experiment 2: File I/O Syscall Intensity

This experiment measures how the number of write syscalls affects file I/O throughput.

The benchmark writes 64MB of data using different chunk sizes. Smaller chunks cause more write syscalls, while larger chunks reduce the syscall count.

### Build and run

Run the following command from the repository root:

./scripts/run_file_io.sh

### Output file

The result is saved to:

results/raw/file_io.csv

### CSV format

total_bytes,chunk_size,write_syscalls,total_ns,seconds,mb_per_sec

### Current result

total_bytes,chunk_size,write_syscalls,total_ns,seconds,mb_per_sec
67108864,1,67108864,20237364093,20.237364,3.16
67108864,4,16777216,5083914169,5.083914,12.59
67108864,16,4194304,1282763334,1.282763,49.89
67108864,64,1048576,326119542,0.326120,196.25
67108864,256,262144,88239792,0.088240,725.30
67108864,1024,65536,26740125,0.026740,2393.41
67108864,4096,16384,14657667,0.014658,4366.32
67108864,16384,4096,7946542,0.007947,8053.82
67108864,65536,1024,8344875,0.008345,7669.38

### Observation

The result shows that very small writes create a large number of syscalls and significantly reduce throughput. Increasing the chunk size reduces syscall count and improves throughput.

### Build and run

Run the following command from the repository root:

./scripts/run_syscall_overhead.sh

### Output file

The result is saved to:

results/raw/syscall_overhead.csv

### CSV format

benchmark,iterations,total_ns,avg_ns

### Current result

benchmark,iterations,total_ns,avg_ns
empty_loop,1000000,248083,0.25
getpid,1000000,97076333,97.08
read_1B_dev_zero,1000000,131142067,131.14
write_1B_dev_null,1000000,126808834,126.81

## Notes

The current environment runs on ARM64/aarch64. Some paper experiments may require x86_64 or kernel-specific features. In that case, the result should be reported as an adapted or partial reproduction.

