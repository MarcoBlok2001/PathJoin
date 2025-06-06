import pandas as pd
import matplotlib.pyplot as plt

# Load data
df_real = pd.read_csv("results/realworld_results.csv")

# Clean column headers
df_real.columns = [col.strip() for col in df_real.columns]

# Group by k and config_id
grouped_real = df_real.groupby(['k', 'config_id']).agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

config_labels = {
    0: 'N1',
    1: 'N2',
    2: 'N3',
    3: 'N4',
    4: 'N5',
}

# Flatten column names
grouped_real.columns = ['k', 'config_id', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']

grouped_real = grouped_real[grouped_real['k'] <= 14]

# -------- Plot 1: Runtime vs k (real world) --------
plt.figure(figsize=(8, 6))
for cid in sorted(grouped_real['config_id'].unique()):
    subset = grouped_real[grouped_real['config_id'] == cid]
    plt.errorbar(
        subset['k'], subset['runtime_mean'], yerr=subset['runtime_std'],
        label=config_labels.get(cid, f'Config {cid}'), marker='o', linestyle='-', capsize=5
    )
plt.title("Real-world: Runtime vs Cycle Length", fontsize=20)
plt.yscale("log")
plt.xlabel("Cycle Length (k)", fontsize=18)
plt.ylabel("Runtime (s)", fontsize=18)
plt.xticks(fontsize=16)
plt.yticks(fontsize=16)
plt.grid(True)
plt.legend(title="Network", fontsize=16, title_fontsize=17)
plt.tight_layout()
plt.savefig("results/realworld_runtime_vs_k.svg", format="svg")

# -------- Plot 2: Cycles vs k (Real world) --------
plt.figure(figsize=(8, 6))
for cid in sorted(grouped_real['config_id'].unique()):
    subset = grouped_real[grouped_real['config_id'] == cid]
    plt.errorbar(
        subset['k'], subset['cycles_mean'], yerr=subset['cycles_std'],
        label=config_labels.get(cid, f'Config {cid}'), marker='o', linestyle='-', capsize=5
    )
plt.title("Real-world: Number of Cycles vs Cycle Length", fontsize=20)
plt.yscale("log")
plt.xlabel("Cycle Length (k)", fontsize=18)
plt.ylabel("# Cycles", fontsize=18)
plt.xticks(fontsize=16)
plt.yticks(fontsize=16)
plt.grid(True)
plt.legend(title="Network", fontsize=16, title_fontsize=17)
plt.tight_layout()
plt.savefig("results/realworld_cycles_vs_k.svg", format="svg")


plt.show()
