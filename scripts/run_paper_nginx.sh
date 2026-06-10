#!/usr/bin/env bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
RESULT_DIR="$PROJECT_ROOT/results/raw"
RESULT_FILE="$RESULT_DIR/paper_nginx_baseline.csv"

WWW_DIR="/tmp/ub_nginx_www"
NGINX_CONF="/tmp/ub_nginx.conf"
NGINX_PID="/tmp/ub_nginx.pid"
PORT=8080

THREADS="${THREADS:-8}"
CONNECTIONS="${CONNECTIONS:-1024}"
DURATION="${DURATION:-12s}"

mkdir -p "$RESULT_DIR"
mkdir -p "$WWW_DIR"

echo "environment,mode,file_size,wrk_threads,connections,duration_seconds,rps" > "$RESULT_FILE"

echo "Creating test files..."
dd if=/dev/zero of="$WWW_DIR/1k.bin" bs=1024 count=1 status=none
dd if=/dev/zero of="$WWW_DIR/4k.bin" bs=1024 count=4 status=none
dd if=/dev/zero of="$WWW_DIR/16k.bin" bs=1024 count=16 status=none
dd if=/dev/zero of="$WWW_DIR/64k.bin" bs=1024 count=64 status=none
dd if=/dev/zero of="$WWW_DIR/256k.bin" bs=1024 count=256 status=none

cat > "$NGINX_CONF" <<CONF
worker_processes auto;
pid $NGINX_PID;
error_log /tmp/ub_nginx_error.log;

events {
    worker_connections 4096;
}

http {
    access_log off;
    sendfile on;
    tcp_nopush on;
    include /etc/nginx/mime.types;

    server {
        listen $PORT;
        location / {
            root $WWW_DIR;
        }
    }
}
CONF

cleanup() {
    echo "Stopping temporary Nginx..."
    nginx -c "$NGINX_CONF" -s stop >/dev/null 2>&1 || true
}
trap cleanup EXIT

ulimit -n 8192 || true

echo "Starting temporary Nginx on port $PORT..."
nginx -c "$NGINX_CONF"
sleep 1

run_one() {
    local file="$1"
    local size="$2"

    echo "Running wrk for $file ..."
    local output
    output="$(wrk -t"$THREADS" -c"$CONNECTIONS" -d"$DURATION" "http://127.0.0.1:$PORT/$file")"

    echo "$output"

    local rps
    rps="$(echo "$output" | awk '/Requests\/sec:/ {print $2}')"

    local duration_seconds
    duration_seconds="${DURATION%s}"

    echo "arta_arm64_vm,baseline,$size,$THREADS,$CONNECTIONS,$duration_seconds,$rps" >> "$RESULT_FILE"
}

run_one "1k.bin" 1024
run_one "4k.bin" 4096
run_one "16k.bin" 16384
run_one "64k.bin" 65536
run_one "256k.bin" 262144

echo "Result saved to $RESULT_FILE"
cat "$RESULT_FILE"
