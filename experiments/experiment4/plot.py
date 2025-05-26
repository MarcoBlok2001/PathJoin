import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load data
df = pd.read_csv("results/degree_results.csv")

# Clean column headers
df.columns = [col.strip() for col in df.columns]

# Group by mean_degree AND variance
grouped = df.groupby(['variance', 'mean_degree']).agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

# Flatten multi-level columns
grouped.columns = ['variance', 'mean_degree', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']

# -------- Plot 1: Runtime vs Mean Degree --------
plt.figure(figsize=(8, 6))
for var in sorted(grouped['variance'].unique()):
    subset = grouped[grouped['variance'] == var]
    plt.errorbar(
        subset['mean_degree'], subset['runtime_mean'], yerr=subset['runtime_std'],
        label=f'Variance {var}', marker='o', linestyle='-', capsize=5
    )
plt.title("Runtime vs Mean Degree (k=6)")
plt.yscale("log")
plt.xlabel("Mean Degree")
plt.ylabel("Runtime (ms)")
plt.legend(title="Variance")
plt.grid(True)
plt.tight_layout()
plt.savefig("results/runtime_vs_mean_degree_by_variance.svg", format="svg")

# -------- Plot 2: Cycles vs Mean Degree --------
plt.figure(figsize=(8, 6))
for var in sorted(grouped['variance'].unique()):
    subset = grouped[grouped['variance'] == var]
    plt.errorbar(
        subset['mean_degree'], subset['cycles_mean'], yerr=subset['cycles_std'],
        label=f'Variance {var}', marker='o', linestyle='-', capsize=5
    )
plt.title("Cycles Found vs Mean Degree (k=6)")
plt.yscale("log")
plt.xlabel("Mean Degree")
plt.ylabel("Number of Cycles")
plt.legend(title="Variance")
plt.grid(True)
plt.tight_layout()
plt.savefig("results/cycles_vs_mean_degree_by_variance.svg", format="svg")

plt.show()
