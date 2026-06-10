# Userspace Bypass: Reproducing Syscall-Intensive Application Experiments

This repository contains an adapted reproduction of experiments from the paper **“Userspace Bypass: Accelerating Syscall-intensive Applications”**.

The goal of this project is to reproduce, document, and compare the paper’s syscall-intensive workloads. The current implementation focuses on baseline measurements on an ARM64 Ubuntu virtual machine, while x86_64 Userspace Bypass / eBPF / XDP comparisons are planned as a separate follow-up on an Intel/Linux machine.

---

## Scope of This Repository

This repository is not a full reimplementation of the paper’s kernel-level Userspace Bypass system. Instead, it provides:

- baseline implementations for the paper’s main workload categories,
- raw CSV result files,
- generated plots,
- reproducibility scripts,
- documentation for the ARM64 VM environment.

The current ARM64 results should be interpreted as **baseline / adapted reproduction results**, not as direct performance claims against the paper’s original x86_64 setup.

---

## Current Reproduction Status

| Paper block | Workload | Current ARM64 status | Result file | Plot |
|---|---|---:|---|---|
| Block 1 | I/O micro-benchmark | Done | `results/raw/paper_io_micro_baseline.csv` | optional / pending |
| Block 2 | Redis GET/SET | Done | `results/raw/paper_redis_baseline.csv` | `results/plots/paper_redis_get_baseline.png`, `results/plots/paper_redis_set_baseline.png` |
| Block 3 | Nginx static files | Done | `results/raw/paper_nginx_baseline.csv` | `results/plots/paper_nginx_baseline.png` |
| Block 4 | Raw UDP socket | Done | `results/raw/paper_raw_socket_baseline.csv` | `results/plots/paper_raw_socket_baseline.png` |

Additional baseline experiments:

| Experiment | Purpose | Result file | Plot |
|---|---|---|---|
| Syscall overhead | Measure basic syscall-related overhead | `results/raw/syscall_overhead.csv` | `results/plots/syscall_overhead.png` |
| File I/O syscall intensity | Measure throughput impact of many small writes | `results/raw/file_io.csv` | `results/plots/file_io_syscalls.png`, `results/plots/file_io_throughput.png` |
| Network I/O syscall intensity | Measure throughput impact of many small sends | `results/raw/network_io.csv` | `results/plots/network_io_syscalls.png`, `results/plots/network_io_throughput.png` |

---

## Experimental Environment

Current environment used for the ARM64 baseline experiments:

| Item | Value |
|---|---|
| Host machine | MacBook |
| Virtualization | Oracle VirtualBox |
| Guest OS | Ubuntu 26.04 |
| Architecture | ARM64 / aarch64 |
| Compiler | GCC |
| Main language | C |
| Scripting | Bash, Python |
| Plotting | Python, pandas, matplotlib |

The exact environment metadata can be saved with:

```bash
./scripts/capture_environment.sh
```

This creates:

```text
results/environment/arta_arm64_vm_environment.txt
```

For more details, see:

```text
docs/reproducibility.md
```

---

## Repository Layout

```text
.
├── benchmarks/
│   ├── file_io/
│   ├── network_io/
│   ├── paper_io_micro/
│   ├── paper_raw_socket/
│   └── syscall_overhead/
├── docs/
│   ├── experiment_matrix.md
│   └── reproducibility.md
├── results/
│   ├── raw/
│   ├── plots/
│   └── environment/
├── scripts/
│   ├── run_paper_io_micro.sh
│   ├── run_paper_redis.sh
│   ├── run_paper_nginx.sh
│   ├── run_paper_raw_socket.sh
│   ├── run_syscall_overhead.sh
│   ├── run_file_io.sh
│   ├── run_network_io.sh
│   ├── plot_paper_redis.py
│   ├── plot_paper_nginx.py
│   ├── plot_paper_raw_socket.py
│   ├── plot_results.py
│   ├── capture_environment.sh
│   └── check_artifacts.sh
└── README.md
```

---

# Paper Block 1: I/O Micro-benchmark Baseline

## Goal

The paper evaluates READ syscall throughput for small buffer sizes and compares normal syscalls, io_uring, and Userspace Bypass.

## Current reproduction

The current ARM64 VM result measures baseline `read()` throughput without Userspace Bypass.

## Environment

| Setting | Value |
|---|---|
| Mode | baseline |
| File location | `/dev/shm` |
| Operation | `read()` |
| Buffer sizes | 64B, 256B, 1KiB, 4KiB |
| Userspace Bypass | not enabled |
| io_uring comparison | pending / optional |

## Run

```bash
./scripts/run_paper_io_micro.sh
```

## Result

```text
results/raw/paper_io_micro_baseline.csv
```

## Interpretation

This experiment shows how small buffer sizes create many syscall invocations and how throughput changes with buffer size. Because this result is collected on ARM64 inside a VM, it should be treated as an adapted baseline rather than a direct comparison with the paper’s original x86_64 results.

---

# Paper Block 2: Redis Baseline

## Goal

The paper evaluates Redis GET/SET throughput for different object sizes and compares normal execution with Userspace Bypass.

## Current reproduction

The current ARM64 VM result measures Redis baseline throughput without Userspace Bypass.

## Environment

| Setting | Value |
|---|---|
| Mode | baseline |
| Redis server | temporary local Redis instance |
| Port | 6380 |
| Requests per data size | 1,000,000 |
| Connections | 50 |
| Threads | 2 |
| Operations | GET, SET |
| Data sizes | 1B, 64B, 256B, 1KiB, 4KiB, 16KiB |

## Run

```bash
./scripts/run_paper_redis.sh
```

## Result

```text
results/raw/paper_redis_baseline.csv
```

## Plots

```text
results/plots/paper_redis_get_baseline.png
results/plots/paper_redis_set_baseline.png
```

## Interpretation

The Redis baseline provides GET/SET request throughput for increasing data sizes. The expected qualitative trend is that throughput changes as object size increases and network / memory transfer costs become more important.

---

# Paper Block 3: Nginx Baseline

## Goal

The paper evaluates Nginx request throughput for static files and compares normal execution with Userspace Bypass.

## Current reproduction

The current ARM64 VM result measures baseline Nginx throughput using `wrk`.

## Environment

| Setting | Value |
|---|---|
| Mode | baseline |
| Nginx | temporary local Nginx instance |
| Load generator | `wrk` |
| Threads | 8 |
| Connections | 1024 |
| Duration | 12 seconds |
| File sizes | 1KiB, 4KiB, 16KiB, 64KiB, 256KiB |

## Run

```bash
./scripts/run_paper_nginx.sh
```

## Result

```text
results/raw/paper_nginx_baseline.csv
```

## Plot

```text
results/plots/paper_nginx_baseline.png
```

## Interpretation

The Nginx baseline shows that request throughput decreases as static file size increases. This matches the expected qualitative trend: small files create high request/syscall pressure, while larger files are more dominated by data transfer cost.

---

# Paper Block 4: Raw UDP Socket Baseline

## Goal

The paper evaluates UDP packet-processing throughput and compares normal socket processing with eBPF/XDP and Userspace Bypass.

## Current reproduction

The current ARM64 VM result measures a baseline UDP loopback sender/receiver benchmark without Userspace Bypass, eBPF, or XDP.

## Environment

| Setting | Value |
|---|---|
| Mode | baseline |
| Protocol | UDP over loopback |
| Duration | 12 seconds |
| Packet sizes | 128B, 512B, 1472B |
| Main metric | sender-side packets per second |

## Run

```bash
./scripts/run_paper_raw_socket.sh
```

## Result

```text
results/raw/paper_raw_socket_baseline.csv
```

## Plot

```text
results/plots/paper_raw_socket_baseline.png
```

## Interpretation

The sender-side UDP baseline shows that packet throughput decreases as packet size increases. This matches the expected qualitative trend for packet-processing workloads: smaller packets create higher packet-rate pressure, while larger packets reduce packet-per-second throughput.

The current ARM64 VM implementation does not include eBPF/XDP or Userspace Bypass. Those comparisons are planned for an x86_64 Linux environment.

---

# Additional Baseline Experiments

## Syscall Overhead

Measures the average cost of basic syscall-related operations.

```bash
./scripts/run_syscall_overhead.sh
```

Result:

```text
results/raw/syscall_overhead.csv
```

## File I/O Syscall Intensity

Measures how small write sizes create more write syscalls and reduce throughput.

```bash
./scripts/run_file_io.sh
```

Result:

```text
results/raw/file_io.csv
```

## Network I/O Syscall Intensity

Measures how small send sizes create more send syscalls and reduce throughput.

```bash
./scripts/run_network_io.sh
```

Result:

```text
results/raw/network_io.csv
```

---

# Reproducing the ARM64 Baselines

## 1. Install dependencies

```bash
sudo apt update
sudo apt install -y git gcc g++ make cmake python3 python3-pandas python3-matplotlib \
    curl wget unzip vim redis-server redis-tools nginx wrk
```

## 2. Clone the repository

```bash
git clone https://github.com/Artakmzh99/Userspace-Bypass-Accelerating-Syscall-intensive-Applications.git
cd Userspace-Bypass-Accelerating-Syscall-intensive-Applications
```

## 3. Capture environment metadata

```bash
./scripts/capture_environment.sh
```

## 4. Run experiments individually

```bash
./scripts/run_paper_io_micro.sh
./scripts/run_paper_redis.sh
./scripts/run_paper_nginx.sh
./scripts/run_paper_raw_socket.sh
```

## 5. Generate plots

```bash
./scripts/plot_paper_redis.py
./scripts/plot_paper_nginx.py
./scripts/plot_paper_raw_socket.py
./scripts/plot_results.py
```

## 6. Check expected artifacts

```bash
./scripts/check_artifacts.sh
```

---

# Notes on ARM64 vs x86_64

The paper’s original evaluation is expected to be closer to a native x86_64 Linux environment with kernel-level support. The current results were collected on ARM64 inside a VM, so they are useful for:

- validating workloads,
- reproducing qualitative trends,
- generating baseline measurements,
- documenting methodology,
- preparing comparison points for x86_64 experiments.

They should not be used as direct replacements for the paper’s original Userspace Bypass measurements.

---

# Current Limitations

- Userspace Bypass is not currently enabled on the ARM64 VM.
- eBPF/XDP comparison is not included in the ARM64 baseline.
- Some kernel-specific artifact experiments may require x86_64 Linux.
- VM results can be affected by virtualization overhead.
- Raw UDP benchmark currently reports sender-side PPS as the primary metric.

---

# Planned x86_64 Follow-up Work

The Intel/x86_64 machine should be used for:

- running the same baseline scripts natively or in an x86_64 Linux VM,
- attempting the original Userspace Bypass artifact,
- collecting UB comparison results if the artifact builds,
- attempting eBPF/XDP packet-processing comparison,
- storing x86_64 environment metadata,
- comparing ARM64 VM baseline results against x86_64 baseline and UB results.

Expected x86_64 deliverables:

```text
results/environment/x86_64_environment.txt
results/raw/x86_64_paper_io_micro_baseline.csv
results/raw/x86_64_paper_redis_baseline.csv
results/raw/x86_64_paper_nginx_baseline.csv
results/raw/x86_64_paper_raw_socket_baseline.csv
docs/x86_64_artifact_notes.md
```

Optional UB/eBPF/XDP deliverables:

```text
results/raw/x86_64_paper_io_micro_ub.csv
results/raw/x86_64_paper_redis_ub.csv
results/raw/x86_64_paper_nginx_ub.csv
results/raw/x86_64_paper_raw_socket_ub.csv
results/raw/x86_64_paper_xdp_baseline.csv
```

---

# Presentation Summary

The current repository demonstrates that the main syscall-intensive workload categories from the paper were reproduced in an adapted baseline form:

1. I/O micro-benchmark
2. Redis
3. Nginx
4. Raw UDP socket processing

For each block, the repository contains source code, run scripts, raw CSV results, and plots where applicable. The next step is to run the original artifact or closer x86_64 reproductions on an Intel/Linux machine and compare the results with the ARM64 VM baselines.