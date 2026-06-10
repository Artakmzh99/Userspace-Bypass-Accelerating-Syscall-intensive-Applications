#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RAW = ROOT / "results" / "raw"
PLOTS = ROOT / "results" / "plots"
PLOTS.mkdir(parents=True, exist_ok=True)

def plot_syscall_overhead():
    path = RAW / "syscall_overhead.csv"
    df = pd.read_csv(path)

    plt.figure()
    plt.bar(df["benchmark"], df["avg_ns"])
    plt.ylabel("Average time (ns)")
    plt.xlabel("Benchmark")
    plt.title("Syscall overhead benchmark")
    plt.xticks(rotation=25, ha="right")
    plt.tight_layout()
    plt.savefig(PLOTS / "syscall_overhead.png", dpi=200)
    plt.close()

def plot_file_io():
    path = RAW / "file_io.csv"
    df = pd.read_csv(path)

    plt.figure()
    plt.plot(df["chunk_size"], df["mb_per_sec"], marker="o")
    plt.xscale("log", base=2)
    plt.xlabel("Chunk size (bytes)")
    plt.ylabel("Throughput (MB/s)")
    plt.title("File I/O throughput vs write chunk size")
    plt.tight_layout()
    plt.savefig(PLOTS / "file_io_throughput.png", dpi=200)
    plt.close()

    plt.figure()
    plt.plot(df["write_syscalls"], df["mb_per_sec"], marker="o")
    plt.xscale("log")
    plt.xlabel("Number of write syscalls")
    plt.ylabel("Throughput (MB/s)")
    plt.title("File I/O throughput vs syscall count")
    plt.tight_layout()
    plt.savefig(PLOTS / "file_io_syscalls.png", dpi=200)
    plt.close()

def plot_network_io():
    path = RAW / "network_io.csv"
    df = pd.read_csv(path)

    plt.figure()
    plt.plot(df["chunk_size"], df["mb_per_sec"], marker="o")
    plt.xscale("log", base=2)
    plt.xlabel("Chunk size (bytes)")
    plt.ylabel("Throughput (MB/s)")
    plt.title("Network I/O throughput vs send chunk size")
    plt.tight_layout()
    plt.savefig(PLOTS / "network_io_throughput.png", dpi=200)
    plt.close()

    plt.figure()
    plt.plot(df["send_syscalls"], df["mb_per_sec"], marker="o")
    plt.xscale("log")
    plt.xlabel("Number of send syscalls")
    plt.ylabel("Throughput (MB/s)")
    plt.title("Network I/O throughput vs syscall count")
    plt.tight_layout()
    plt.savefig(PLOTS / "network_io_syscalls.png", dpi=200)
    plt.close()

def main():
    plot_syscall_overhead()
    plot_file_io()
    plot_network_io()
    print(f"Plots saved to: {PLOTS}")

if __name__ == "__main__":
    main()
