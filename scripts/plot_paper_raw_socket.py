#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RAW = ROOT / "results" / "raw"
PLOTS = ROOT / "results" / "plots"
PLOTS.mkdir(parents=True, exist_ok=True)

df = pd.read_csv(RAW / "paper_raw_socket_baseline.csv")
df = df.sort_values("packet_size")

plt.figure()
plt.plot(df["packet_size"], df["sent_pps"], marker="o")
plt.xscale("log", base=2)
plt.xlabel("Packet size (bytes)")
plt.ylabel("Packets per second, sender-side")
plt.title("Paper Block 4: Raw UDP socket baseline")
plt.tight_layout()
plt.savefig(PLOTS / "paper_raw_socket_baseline.png", dpi=200)
plt.close()

print(f"Plot saved to {PLOTS / 'paper_raw_socket_baseline.png'}")
