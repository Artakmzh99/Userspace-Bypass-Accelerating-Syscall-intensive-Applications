#!/usr/bin/env bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BENCH_DIR="$PROJECT_ROOT/benchmarks/paper_raw_socket"
RESULT_DIR="$PROJECT_ROOT/results/raw"
RESULT_FILE="$RESULT_DIR/paper_raw_socket_baseline.csv"

mkdir -p "$RESULT_DIR"

cd "$BENCH_DIR"
make clean
make

./udp_pps_bench 12 > "$RESULT_FILE"

echo "Result saved to $RESULT_FILE"
cat "$RESULT_FILE"
