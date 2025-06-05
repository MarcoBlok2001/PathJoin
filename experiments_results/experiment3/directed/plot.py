import pandas as pd
import matplotlib.pyplot as plt

# Load data
df_dir = pd.read_csv("results/directed_results.csv")

# Clean column headers
df_dir.columns = [col.strip() for col in df_dir.columns]

# Group by k and config_id
grouped_dir = df_dir.groupby(['k', 'config_id']).agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

config_labels = {
    0: '(1000v,2000e)',
    1: '(1000v,2500e)',
    2: '(1000v,3000e)',
    3: '(2000v,4000e)',
    4: '(3000v,6000e)',
}


# Flatten column names
grouped_dir.columns = ['k', 'config_id', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']

# -------- Plot 3: Runtime vs k (Directed) --------
plt.figure(figsize=(8, 6))
for cid in sorted(grouped_dir['config_id'].unique()):
    subset = grouped_dir[grouped_dir['config_id'] == cid]
    plt.errorbar(
        subset['k'], subset['runtime_mean'], yerr=subset['runtime_std'],
        label=config_labels.get(cid, f'Config {cid}'), marker='o', linestyle='-', capsize=5
    )
plt.title("Directed: Runtime vs Cycle Length", fontsize=16)
plt.yscale("log")
plt.xlabel("Cycle Length (k)", fontsize=14)
plt.ylabel("Runtime (s)", fontsize=14)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)
plt.grid(True)
plt.legend(title="Graph Config", fontsize=12, title_fontsize=13)
plt.tight_layout()
plt.savefig("results/directed_runtime_vs_k.svg", format="svg")

# -------- Plot 4: Cycles vs k (Directed) --------
plt.figure(figsize=(8, 6))
for cid in sorted(grouped_dir['config_id'].unique()):
    subset = grouped_dir[grouped_dir['config_id'] == cid]
    plt.errorbar(
        subset['k'], subset['cycles_mean'], yerr=subset['cycles_std'],
        label=config_labels.get(cid, f'Config {cid}'), marker='o', linestyle='-', capsize=5
    )
plt.title("Directed: Number of Cycles vs Cycle Length", fontsize=16)
plt.yscale("log")
plt.xlabel("Cycle Length (k)", fontsize=14)
plt.ylabel("# Cycles", fontsize=14)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)
plt.grid(True)
plt.legend(title="Graph Config", fontsize=12, title_fontsize=13)
plt.tight_layout()
plt.savefig("results/directed_cycles_vs_k.svg", format="svg")

plt.show()
