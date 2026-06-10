# Reproducibility Notes

This document explains how the ARM64 baseline experiments were executed and what metadata should be stored for evaluation.

---

## ARM64 Baseline Environment

The current experiments were executed on:

| Item | Value |
|---|---|
| Host | MacBook |
| Virtualization | Oracle VirtualBox |
| Guest OS | Ubuntu 26.04 |
| Architecture | ARM64 / aarch64 |
| Compiler | GCC |
| Main tools | Redis, Nginx, wrk, Python, pandas, matplotlib |

Because this setup is a virtualized ARM64 environment, the results are considered adapted baseline measurements.

---

## Why Store Environment Metadata?

Performance results are highly dependent on:

- CPU architecture,
- kernel version,
- VM configuration,
- available memory,
- compiler version,
- Redis version,
- Nginx version,
- wrk version,
- background system load.

For this reason, the repository stores environment metadata in:
results/environment/arta_arm64_vm_environment.txt

---

## Capturing Environment Metadata

Run from the repository root:
./scripts/capture_environment.sh

This script records:

- uname -a
- /etc/os-release
- lscpu
- memory information
- disk information
- GCC version
- Python version
- Git version
- Redis version
- Nginx version
- wrk version
- current Git commit

---

## Recommended Experiment Procedure

For each benchmark:

1. Close unnecessary applications in the VM.
2. Disable VM audio if it creates notification noise.
3. Make sure the VM has stable CPU and memory allocation.
4. Run the benchmark once as a warm-up if needed.
5. Run the official script.
6. Store CSV output under results/raw/.
7. Generate plots under results/plots/.
8. Commit source code, scripts, results, plots, and documentation.

---

## Current ARM64 Benchmark Scripts
./scripts/run_paper_io_micro.sh
./scripts/run_paper_redis.sh
./scripts/run_paper_nginx.sh
./scripts/run_paper_raw_socket.sh

---

## Plot Scripts
./scripts/plot_paper_redis.py
./scripts/plot_paper_nginx.py
./scripts/plot_paper_raw_socket.py

---

## Artifact Check

Run:
./scripts/check_artifacts.sh

This checks whether the most important result files and plots exist.

---

## Interpreting Results

The ARM64 VM results should be interpreted as:

- workload validation,
- baseline results,
- qualitative reproduction,
- preparation for x86_64 comparison.

They should not be interpreted as direct replacements for the paper’s original Userspace Bypass results.

---

## Recommended Future Improvement

A useful next step is to repeat each experiment three times and report:

- mean,
- min,
- max,
- standard deviation.

This would reduce noise and make the presentation stronger.