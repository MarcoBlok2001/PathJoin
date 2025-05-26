import pandas as pd
import matplotlib.pyplot as plt

# Load data
df_undir = pd.read_csv("results/undirected_results.csv")
df_dir = pd.read_csv("results/directed_results.csv")

# Clean column headers
df_undir.columns = [col.strip() for col in df_undir.columns]
df_dir.columns = [col.strip() for col in df_dir.columns]

# Group by k and join
grouped_undir = df_undir.groupby(['k', 'join']).agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

grouped_dir = df_dir.groupby(['k', 'join']).agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

# Flatten column names
grouped_undir.columns = ['k', 'join', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']
grouped_dir.columns = ['k', 'join', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']

# -------- Plot 1: Runtime vs k (Undirected) --------
plt.figure(figsize=(8, 6))
for j in sorted(grouped_undir['join'].unique()):
    subset = grouped_undir[grouped_undir['join'] == j]
    plt.errorbar(
        subset['k'], subset['runtime_mean'], yerr=subset['runtime_std'],
        label=f'Join {j}', marker='o', linestyle='-', capsize=5
    )
plt.title("Undirected: Runtime vs k")
plt.yscale("log")
plt.xlabel("k")
plt.ylabel("Runtime (ms)")
plt.grid(True)
plt.legend(title="Join")
plt.tight_layout()
plt.savefig("results/undirected_runtime_vs_k.svg", format="svg")

# -------- Plot 2: Cycles vs k (Undirected) --------
plt.figure(figsize=(8, 6))
for j in sorted(grouped_undir['join'].unique()):
    subset = grouped_undir[grouped_undir['join'] == j]
    plt.errorbar(
        subset['k'], subset['cycles_mean'], yerr=subset['cycles_std'],
        label=f'Join {j}', marker='o', linestyle='-', capsize=5
    )
plt.title("Undirected: Cycles vs k")
plt.yscale("log")
plt.xlabel("k")
plt.ylabel("Cycles")
plt.grid(True)
plt.legend(title="Join")
plt.tight_layout()
plt.savefig("results/undirected_cycles_vs_k.svg", format="svg")

# -------- Plot 3: Runtime vs k (Directed) --------
plt.figure(figsize=(8, 6))
for j in sorted(grouped_dir['join'].unique()):
    subset = grouped_dir[grouped_dir['join'] == j]
    plt.errorbar(
        subset['k'], subset['runtime_mean'], yerr=subset['runtime_std'],
        label=f'Join {j}', marker='o', linestyle='-', capsize=5
    )
plt.title("Directed: Runtime vs k")
plt.yscale("log")
plt.xlabel("k")
plt.ylabel("Runtime (ms)")
plt.grid(True)
plt.legend(title="Join")
plt.tight_layout()
plt.savefig("results/directed_runtime_vs_k.svg", format="svg")

# -------- Plot 4: Cycles vs k (Directed) --------
plt.figure(figsize=(8, 6))
for j in sorted(grouped_dir['join'].unique()):
    subset = grouped_dir[grouped_dir['join'] == j]
    plt.errorbar(
        subset['k'], subset['cycles_mean'], yerr=subset['cycles_std'],
        label=f'Join {j}', marker='o', linestyle='-', capsize=5
    )
plt.title("Directed: Cycles vs k")
plt.yscale("log")
plt.xlabel("k")
plt.ylabel("Cycles")
plt.grid(True)
plt.legend(title="Join")
plt.tight_layout()
plt.savefig("results/directed_cycles_vs_k.svg", format="svg")

plt.show()
