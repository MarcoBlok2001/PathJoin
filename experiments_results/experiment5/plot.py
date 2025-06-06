import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV file
df = pd.read_csv("results/results.csv")

# Clean column headers and values
df.columns = [col.strip() for col in df.columns]
df['2core_flag'] = df['2core_flag'].astype(str).str.strip().str.lower()

# Map real-world graph_ids to filenames
real_graph_map = {
    # 10: "david_copperfield.net",
    10: "N2",
    11: "N5",
}

# Add graph_type and real_world_name columns
df['graph_type'] = np.where(df['graph_id'] < 10, 'Random', 'Real-world')
df['graph_name'] = df['graph_id'].map(real_graph_map).fillna('Random Avg')

# Group data
# 1. For Random: aggregate across all random graph_ids
random_df = df[df['graph_type'] == 'Random']
random_grouped = random_df.groupby(['2core_flag', 'k'])['runtime_ms'].agg(['mean', 'std']).reset_index()
random_grouped['graph_name'] = 'Synthetic'

# 2. For each real-world graph: keep them separate
real_df = df[df['graph_type'] == 'Real-world']
real_grouped = real_df.groupby(['graph_name', '2core_flag', 'k'])['runtime_ms'].agg(['mean', 'std']).reset_index()

# Combine for plotting
combined = pd.concat([random_grouped.rename(columns={'2core_flag': 'flag'}),
                      real_grouped.rename(columns={'2core_flag': 'flag'})],
                     ignore_index=True)

# Plotting
plt.figure(figsize=(8, 6))

colors = plt.cm.tab10.colors
graph_names = combined['graph_name'].unique()
color_map = {name: colors[i % len(colors)] for i, name in enumerate(graph_names)}

for name in graph_names:
    for flag in ['false', 'true']:
        data = combined[(combined['graph_name'] == name) & (combined['flag'] == flag)]
        if data.empty:
            continue
        label = f"{name} - {'2-core' if flag == 'true' else 'No 2-core'}"
        linestyle = '-' if flag == 'true' else '--'
        plt.errorbar(
            data['k'], data['mean'], yerr=data['std'],
            label=label, marker='o', capsize=5,
            linestyle=linestyle, color=color_map[name]
        )

# Plot aesthetics
plt.title("Runtime vs Cycle Length (k)", fontsize=18)
plt.yscale("log")
plt.xlabel("Cycle Length (k)", fontsize=16)
plt.ylabel("Runtime (s)", fontsize=16)
plt.xticks(fontsize=14)
plt.yticks(fontsize=14)
# plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.grid(True)
plt.legend(title="Graph", fontsize=14, title_fontsize=15)
plt.tight_layout()

# Save and show
plt.savefig("results/runtime_vs_k_2core.svg", format="svg")
plt.show()
