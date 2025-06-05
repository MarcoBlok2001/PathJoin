import pandas as pd
import matplotlib.pyplot as plt

# Load data
df_undir = pd.read_csv("results/undirected_results.csv")

# Clean column headers
df_undir.columns = [col.strip() for col in df_undir.columns]

# Group by k and config_id
grouped_undir = df_undir.groupby(['k', 'config_id']).agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

config_labels = {
    0: '(1000v,1250e)',
    1: '(1000v,1500e)',
    2: '(1000v,1750e)',
    3: '(1500v,1875e)',
    4: '(2000v,2500e)',
}

# Flatten column names
grouped_undir.columns = ['k', 'config_id', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']

# -------- Plot 1: Runtime vs k (Undirected) --------
plt.figure(figsize=(8, 6))
for cid in sorted(grouped_undir['config_id'].unique()):
    subset = grouped_undir[grouped_undir['config_id'] == cid]
    plt.errorbar(
        subset['k'], subset['runtime_mean'], yerr=subset['runtime_std'],
        label=config_labels.get(cid, f'Config {cid}'), marker='o', linestyle='-', capsize=5
    )
plt.title("Undirected: Runtime vs Cycle Length", fontsize=16)
plt.yscale("log")
plt.xlabel("Cycle Length (k)", fontsize=14)
plt.ylabel("Runtime (s)", fontsize=14)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)
plt.grid(True)
plt.legend(title="Graph Config", fontsize=12, title_fontsize=13)
plt.tight_layout()
plt.savefig("results/undirected_runtime_vs_k.svg", format="svg")

# -------- Plot 2: Cycles vs k (Undirected) --------
plt.figure(figsize=(8, 6))
for cid in sorted(grouped_undir['config_id'].unique()):
    subset = grouped_undir[grouped_undir['config_id'] == cid]
    plt.errorbar(
        subset['k'], subset['cycles_mean'], yerr=subset['cycles_std'],
        label=config_labels.get(cid, f'Config {cid}'), marker='o', linestyle='-', capsize=5
    )
plt.title("Undirected: Number of Cycles vs Cycle Length", fontsize=16)
plt.yscale("log")
plt.xlabel("Cycle Length (k)", fontsize=14)
plt.ylabel("# Cycles", fontsize=14)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)
plt.grid(True)
plt.legend(title="Graph Config", fontsize=12, title_fontsize=13)
plt.tight_layout()
plt.savefig("results/undirected_cycles_vs_k.svg", format="svg")

plt.show()
