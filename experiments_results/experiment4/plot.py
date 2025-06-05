import pandas as pd
import matplotlib.pyplot as plt

# Load synthetic results
df = pd.read_csv("results/degree_results.csv")

# Clean column names
df.columns = [col.strip() for col in df.columns]

# Ensure correct types
df['graph_type'] = df['graph_type'].astype(int)
df['k'] = df['k'].astype(int)

# Group and aggregate
grouped = df.groupby(['graph_type', 'k', 'mean_degree']).agg({
    'runtime_ms': ['mean', 'std'],
    'cycles': ['mean', 'std']
}).reset_index()

# Flatten column names
grouped.columns = ['graph_type', 'k', 'mean_degree', 'runtime_mean', 'runtime_std', 'cycles_mean', 'cycles_std']

# Titles for plot labeling
type_labels = {
    0: "Normal Degree Distribution",
    1: "Scale-Free (Barabási–Albert)"
}

# ------------- Plot 1: Runtime vs Mean Degree -------------
for gtype in [0, 1]:
    plt.figure(figsize=(8, 6))
    subset = grouped[grouped['graph_type'] == gtype]
    for k_val in sorted(subset['k'].unique()):
        k_data = subset[subset['k'] == k_val]
        plt.errorbar(
            k_data['mean_degree'], k_data['runtime_mean'], yerr=k_data['runtime_std'],
            label=f'k={k_val}', marker='o', linestyle='-', capsize=4
        )
    plt.title(f"{type_labels[gtype]}: Runtime vs Mean Degree", fontsize=16)
    plt.xlabel("Mean Degree", fontsize=14)
    plt.ylabel("Runtime (s)", fontsize=14)
    plt.yscale("log")
    plt.grid(True)
    plt.legend(title="Cycle Length k", fontsize=12, title_fontsize=13)
    plt.tight_layout()
    plt.savefig(f"results/{'normal' if gtype == 0 else 'scalefree'}_runtime_vs_mean.svg", format="svg")

# ------------- Plot 2: Cycles vs Mean Degree -------------
for gtype in [0, 1]:
    plt.figure(figsize=(8, 6))
    subset = grouped[grouped['graph_type'] == gtype]
    for k_val in sorted(subset['k'].unique()):
        k_data = subset[subset['k'] == k_val]
        plt.errorbar(
            k_data['mean_degree'], k_data['cycles_mean'], yerr=k_data['cycles_std'],
            label=f'k={k_val}', marker='o', linestyle='-', capsize=4
        )
    plt.title(f"{type_labels[gtype]}: #Cycles vs Mean Degree", fontsize=16)
    plt.xlabel("Mean Degree", fontsize=14)
    plt.ylabel("Number of Cycles", fontsize=14)
    plt.yscale("log")
    plt.grid(True)
    plt.legend(title="Cycle Length k", fontsize=12, title_fontsize=13)
    plt.tight_layout()
    plt.savefig(f"results/{'normal' if gtype == 0 else 'scalefree'}_cycles_vs_mean.svg", format="svg")

plt.show()
