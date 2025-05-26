import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the data
df = pd.read_csv("results/p_results.csv")

# Clean and convert p coefficient to float (may be string if read from CSV with 0.0 etc.)
df['p'] = df['p'].astype(float)

# Group by p coefficient
grouped = df.groupby('p').agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

# Flatten column names
grouped.columns = ['p', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']

# ------------------ Plot 1: Runtime vs P Coefficient ------------------
plt.figure(figsize=(8, 6))
plt.errorbar(
    grouped['p'], grouped['runtime_mean'], yerr=grouped['runtime_std'],
    marker='o', linestyle='-', capsize=5
)
plt.title("Runtime vs P Coefficient (k=6)")
plt.xlabel("P Coefficient (p)")
plt.ylabel("Runtime (ms)")
plt.grid(True)
plt.tight_layout()
plt.savefig("results/runtime_vs_p.svg", format="svg")

# ------------------ Plot 2: Cycles vs P Coefficient ------------------
plt.figure(figsize=(8, 6))
plt.errorbar(
    grouped['p'], grouped['cycles_mean'], yerr=grouped['cycles_std'],
    marker='o', linestyle='-', capsize=5
)
plt.title("Cycles Found vs P Coefficient (k=6)")
plt.xlabel("P Coefficient (p)")
plt.ylabel("Number of Cycles")
plt.grid(True)
plt.tight_layout()
plt.savefig("results/cycles_vs_p.svg", format="svg")

plt.show()
