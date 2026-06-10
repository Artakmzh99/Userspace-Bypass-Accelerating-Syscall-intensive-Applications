#!/usr/bin/env bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BENCH_DIR="$PROJECT_ROOT/benchmarks/paper_io_micro"
RESULT_DIR="$PROJECT_ROOT/results/raw"

mkdir -p "$RESULT_DIR"

cd "$BENCH_DIR"
make clean
make

./read_iops_bench 2000000 > "$RESULT_DIR/paper_io_micro_baseline.csv"

echo "Result saved to $RESULT_DIR/paper_io_micro_baseline.csv"
cat "$RESULT_DIR/paper_io_micro_baseline.csv"
