import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV file
df = pd.read_csv("results/results.csv")

# Clean column headers and values
df.columns = [col.strip() for col in df.columns]
df['2core_flag'] = df['2core_flag'].astype(str).str.strip()

# Group by 2core_flag and k, calculate mean and std deviation of runtime
grouped = df.groupby(['2core_flag', 'k'])['runtime_ms'].agg(['mean', 'std']).reset_index()

# Start plotting
plt.figure(figsize=(10, 6))

for flag in ['false', 'true']:
    data = grouped[grouped['2core_flag'] == flag]
    plt.errorbar(
        data['k'], data['mean'], yerr=data['std'],
        label='With 2-core' if flag == 'true' else 'Without 2-core',
        marker='o', capsize=5, linestyle='-'
    )

plt.yscale("log")

# Formatting the plot
plt.title("Average Runtime vs k (With and Without 2-core Preprocessing)")
plt.xlabel("k Value")
plt.ylabel("Runtime (ms)")
plt.grid(True)
plt.legend()
plt.xticks(np.arange(df['k'].min(), df['k'].max() + 1, 1))

# Save as vector image
plt.tight_layout()
plt.savefig("results/runtime_vs_k.svg", format="svg")
plt.show()
