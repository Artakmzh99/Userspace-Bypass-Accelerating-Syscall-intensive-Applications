#!/usr/bin/env bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT_DIR="$PROJECT_ROOT/results/environment"
OUT_FILE="$OUT_DIR/arta_arm64_vm_environment.txt"

mkdir -p "$OUT_DIR"

{
  echo "=== date ==="
  date
  echo

  echo "=== git commit ==="
  git -C "$PROJECT_ROOT" rev-parse HEAD || true
  echo

  echo "=== uname -a ==="
  uname -a
  echo

  echo "=== os release ==="
  cat /etc/os-release || true
  echo

  echo "=== architecture ==="
  uname -m
  echo

  echo "=== lscpu ==="
  lscpu || true
  echo

  echo "=== memory ==="
  free -h || true
  echo

  echo "=== gcc ==="
  gcc --version || true
  echo

  echo "=== python ==="
  python3 --version || true
  echo

  echo "=== git ==="
  git --version || true
  echo

  echo "=== redis ==="
  redis-server --version 2>&1 || true
  echo

  echo "=== nginx ==="
  nginx -v 2>&1 || true
  echo

  echo "=== wrk ==="
  wrk --version 2>&1 || true
} > "$OUT_FILE"

echo "Environment metadata saved to $OUT_FILE"
