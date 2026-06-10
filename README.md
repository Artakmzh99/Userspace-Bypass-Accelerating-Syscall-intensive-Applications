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