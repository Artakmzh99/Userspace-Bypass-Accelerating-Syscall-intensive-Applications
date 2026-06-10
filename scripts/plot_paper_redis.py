#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RAW = ROOT / "results" / "raw"
PLOTS = ROOT / "results" / "plots"
PLOTS.mkdir(parents=True, exist_ok=True)

df = pd.read_csv(RAW / "paper_redis_baseline.csv")

for op in ["GET", "SET"]:
    sub = df[df["operation"] == op].copy()
    sub = sub.sort_values("data_size")

    plt.figure()
    plt.plot(sub["data_size"], sub["rps"], marker="o")
    plt.xscale("log", base=2)
    plt.xlabel("Data size (bytes)")
    plt.ylabel("Requests per second")
    plt.title(f"Paper Block 2: Redis {op} baseline")
    plt.tight_layout()
    plt.savefig(PLOTS / f"paper_redis_{op.lower()}_baseline.png", dpi=200)
    plt.close()

print(f"Plots saved to {PLOTS}")
