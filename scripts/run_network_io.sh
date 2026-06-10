#!/usr/bin/env bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BENCH_DIR="$PROJECT_ROOT/benchmarks/network_io"
RESULT_DIR="$PROJECT_ROOT/results/raw"

mkdir -p "$RESULT_DIR"

cd "$BENCH_DIR"
make clean
make

./network_io_bench 67108864 > "$RESULT_DIR/network_io.csv"

echo "Result saved to $RESULT_DIR/network_io.csv"
cat "$RESULT_DIR/network_io.csv"
