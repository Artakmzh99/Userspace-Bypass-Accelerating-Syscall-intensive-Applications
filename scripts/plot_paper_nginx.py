#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RAW = ROOT / "results" / "raw"
PLOTS = ROOT / "results" / "plots"
PLOTS.mkdir(parents=True, exist_ok=True)

df = pd.read_csv(RAW / "paper_nginx_baseline.csv")
df = df.sort_values("file_size")

plt.figure()
plt.plot(df["file_size"], df["rps"], marker="o")
plt.xscale("log", base=2)
plt.xlabel("File size (bytes)")
plt.ylabel("Requests per second")
plt.title("Paper Block 3: Nginx baseline")
plt.tight_layout()
plt.savefig(PLOTS / "paper_nginx_baseline.png", dpi=200)
plt.close()

print(f"Plot saved to {PLOTS / 'paper_nginx_baseline.png'}")
