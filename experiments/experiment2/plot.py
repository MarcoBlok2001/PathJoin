import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV file
df = pd.read_csv("results/undirected_split_results.csv")

# Group by split configuration
grouped = df.groupby('c_args').agg({
    'runtime_ms': ['mean', 'std']
}).reset_index()

# Prepare data for plotting
splits = grouped['c_args']
split_labels = [s.replace(' ', '-') for s in splits]
means = grouped[('runtime_ms', 'mean')]
stds = grouped[('runtime_ms', 'std')]

x = np.arange(len(split_labels))
width = 0.6

# Plot
plt.figure(figsize=(8, 6))
plt.bar(x, means, width, yerr=stds, capsize=5, color='steelblue')
plt.xticks(x, split_labels)
plt.title("Runtime vs Split Configuration (k=12)")
plt.xlabel("Split (e.g. 1-5 means c=1,5)")
plt.ylabel("Mean Runtime (ms)")
plt.tight_layout()
plt.savefig("results/runtime_vs_split.svg", format="svg")
plt.show()
