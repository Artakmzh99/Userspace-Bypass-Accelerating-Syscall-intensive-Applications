#!/usr/bin/env bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
RESULT_DIR="$PROJECT_ROOT/results/raw"
RESULT_FILE="$RESULT_DIR/paper_redis_baseline.csv"

PORT=6380
REQUESTS="${REQUESTS:-1000000}"
CONNECTIONS="${CONNECTIONS:-50}"
THREADS="${THREADS:-2}"

mkdir -p "$RESULT_DIR"

echo "environment,mode,operation,data_size,requests,connections,threads,rps" > "$RESULT_FILE"

echo "Starting temporary Redis server on port $PORT..."
redis-server \
  --port "$PORT" \
  --save "" \
  --appendonly no \
  --daemonize yes \
  --dir /tmp \
  --pidfile /tmp/redis-paper-benchmark.pid

sleep 1

cleanup() {
  echo "Stopping temporary Redis server..."
  redis-cli -p "$PORT" shutdown nosave >/dev/null 2>&1 || true
}
trap cleanup EXIT

for SIZE in 1 64 256 1024 4096 16384; do
  echo "Running Redis benchmark for data size ${SIZE} bytes..."

  redis-benchmark \
    -p "$PORT" \
    -t get,set \
    -n "$REQUESTS" \
    -d "$SIZE" \
    -c "$CONNECTIONS" \
    --threads "$THREADS" \
    --csv > /tmp/redis_bench_${SIZE}.csv

  awk -F',' -v size="$SIZE" -v req="$REQUESTS" -v conn="$CONNECTIONS" -v threads="$THREADS" '
    NR > 1 {
      op=$1
      rps=$2
      gsub(/"/, "", op)
      gsub(/"/, "", rps)
      print "arta_arm64_vm,baseline," op "," size "," req "," conn "," threads "," rps
    }
  ' /tmp/redis_bench_${SIZE}.csv >> "$RESULT_FILE"
done

echo "Result saved to $RESULT_FILE"
cat "$RESULT_FILE"
