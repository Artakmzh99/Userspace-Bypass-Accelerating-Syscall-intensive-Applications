#!/usr/bin/env bash
set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$PROJECT_ROOT"

missing=0

check_file() {
  local path="$1"
  if [ -f "$path" ]; then
    echo "[OK]      $path"
  else
    echo "[MISSING] $path"
    missing=1
  fi
}

echo "Checking raw result files..."
check_file "results/raw/paper_io_micro_baseline.csv"
check_file "results/raw/paper_redis_baseline.csv"
check_file "results/raw/paper_nginx_baseline.csv"
check_file "results/raw/paper_raw_socket_baseline.csv"

echo
echo "Checking plot files..."
check_file "results/plots/paper_redis_get_baseline.png"
check_file "results/plots/paper_redis_set_baseline.png"
check_file "results/plots/paper_nginx_baseline.png"
check_file "results/plots/paper_raw_socket_baseline.png"

echo
echo "Checking documentation files..."
check_file "README.md"
check_file "docs/experiment_matrix.md"
check_file "docs/reproducibility.md"
check_file "docs/x86_64_fredi_tasks.md"

echo
echo "Checking run scripts..."
check_file "scripts/run_paper_io_micro.sh"
check_file "scripts/run_paper_redis.sh"
check_file "scripts/run_paper_nginx.sh"
check_file "scripts/run_paper_raw_socket.sh"
check_file "scripts/capture_environment.sh"

if [ "$missing" -eq 0 ]; then
  echo
  echo "All required artifacts are present."
else
  echo
  echo "Some artifacts are missing. Check the [MISSING] entries above."
  exit 1
fi