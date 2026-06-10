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

  echo "=== /etc/os-release ==="
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

  echo "=== disk ==="
  df -h || true
  echo

  echo "=== gcc --version ==="
  gcc --version || true
  echo

  echo "=== g++ --version ==="
  g++ --version || true
  echo

  echo "=== make --version ==="
  make --version || true
  echo

  echo "=== python3 --version ==="
  python3 --version || true
  echo

  echo "=== pip3 --version ==="
  pip3 --version || true
  echo

  echo "=== git --version ==="
  git --version || true
  echo

  echo "=== redis-server --version ==="
  redis-server --version 2>&1 || true
  echo

  echo "=== nginx -v ==="
  nginx -v 2>&1 || true
  echo

  echo "=== wrk --version ==="
  wrk --version 2>&1 || true
  echo

  echo "=== Python packages ==="
  python3 - <<'PY' || true
try:
    import pandas
    print("pandas", pandas.__version__)
except Exception as exc:
    print("pandas unavailable:", exc)

try:
    import matplotlib
    print("matplotlib", matplotlib.__version__)
except Exception as exc:
    print("matplotlib unavailable:", exc)
PY
} > "$OUT_FILE"

echo "Environment metadata saved to $OUT_FILE"
