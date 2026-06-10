#!/usr/bin/env bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BENCH_DIR="$PROJECT_ROOT/benchmarks/syscall_overhead"
RESULT_DIR="$PROJECT_ROOT/results/raw"

mkdir -p "$RESULT_DIR"

cd "$BENCH_DIR"
make clean
make

./syscall_bench 1000000 > "$RESULT_DIR/syscall_overhead.csv"

echo "Result saved to $RESULT_DIR/syscall_overhead.csv"
cat "$RESULT_DIR/syscall_overhead.csv"
