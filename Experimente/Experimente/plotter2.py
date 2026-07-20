import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np

# ── Daten einlesen ──────────────────────────────────────────────────────────
df = pd.read_csv("Experimente\Experimente\Raw-socket\kpti-off\socket-without_sum.csv", index_col=0)

io_sizes = df.index.tolist()
base     = df["Base"].values
ub       = df["UB"].values
eBPF = df["eBPF"].values

# ── Farben & Marker (wie im Paper) ──────────────────────────────────────────
COLOR_URING = "#1f77b4"
COLOR_UB    = "#2ca02c"
COLOR_BASE  = "#9467bd"

x = np.arange(len(io_sizes))

# ── Figure ───────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(5, 4))

ax.plot(x, eBPF, color=COLOR_URING, marker="*", markersize=8, linewidth=1.5, label="eBPF")
ax.plot(x, ub,       color=COLOR_UB,    marker="x", markersize=8, linewidth=1.5, label="UB")
ax.plot(x, base,     color=COLOR_BASE,  marker="D", markersize=6, linewidth=1.5, label="base")

# ── Beschleunigungsrate als Label über UB-Linie ──────────────────────────────
for xi, (bv, uv) in enumerate(zip(base, ub)):
    pct = (uv - bv) / bv * 100
    sign = "+" if pct >= 0 else ""
    ax.annotate(f"{sign}{pct:.1f}%",
                xy=(xi, uv),
                xytext=(0, 7),
                textcoords="offset points",
                ha="center", fontsize=8,
                color=COLOR_UB)

# ── Achsen & Layout ──────────────────────────────────────────────────────────
ax.set_xticks(x)
ax.set_xticklabels(io_sizes, fontsize=9)
ax.set_xlabel("Packet Size", fontsize=10)
ax.set_ylabel("PPS(K)", fontsize=10)
ax.set_title("KPTI=on", fontsize=10)
ax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda v, _: f"{int(v):,}"))
ax.set_ylim(bottom=0)
ax.grid(axis="y", linestyle="--", alpha=0.4)
ax.legend(fontsize=9, frameon=True)

plt.tight_layout()
plt.savefig("Experimente\Experimente\Raw-Socket_withoutsum_benchmark_plot.png", dpi=150, bbox_inches="tight")
print("Plot gespeichert: io_benchmark_plot.png")