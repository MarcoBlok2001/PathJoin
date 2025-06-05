import pandas as pd
import matplotlib.pyplot as plt

# Load data
df_undir = pd.read_csv("results/undirected_results.csv")

# Clean column headers
df_undir.columns = [col.strip() for col in df_undir.columns]

# Group by k and join
grouped_undir = df_undir.groupby(['k', 'join']).agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

joinmethod_labels = {
    2: '2-join',
    3: '3-join',
    4: '4-join',
}

# Flatten column names
grouped_undir.columns = ['k', 'join', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']

# -------- Plot 1: Runtime vs k (Undirected) --------
plt.figure(figsize=(8, 6))
for j in sorted(grouped_undir['join'].unique()):
    subset = grouped_undir[grouped_undir['join'] == j]
    plt.errorbar(
        subset['k'], subset['runtime_mean'], yerr=subset['runtime_std'],
        label=joinmethod_labels.get(j, f'Config {j}'), marker='o', linestyle='-', capsize=5
    )
plt.title("Runtime vs Cycle Length",fontsize=16)
plt.yscale("log")
plt.xlabel("Cycle Length (k)", fontsize=14)
plt.ylabel("Runtime (s)", fontsize=14)
plt.grid(True)
plt.legend(title="Join method", fontsize=12, title_fontsize=13)
plt.tight_layout()
plt.savefig("results/joinmethods_runtime_vs_k.svg", format="svg")


plt.show()
