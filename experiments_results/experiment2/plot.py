import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def plot_runtime_by_split(file_path, color, title, output_file):
    # Load CSV
    df = pd.read_csv(file_path)

    # Get unique sorted split configurations from c_args
    split_order = sorted(df['c_args'].unique(), key=lambda s: list(map(int, s.split())))
    df['c_args'] = pd.Categorical(df['c_args'], categories=split_order, ordered=True)

    # Group by configuration
    grouped = df.groupby('c_args', sort=False)['runtime_ms'].agg(['mean', 'std']).reset_index()

    # Prepare plot data
    labels = [s.replace(' ', '-') for s in grouped['c_args']]
    x = np.arange(len(labels))

    # Plot
    plt.figure(figsize=(8, 6))
    plt.bar(x, grouped['mean'], 0.6, yerr=grouped['std'], capsize=5, color=color)
    plt.xticks(x, labels, fontsize=18)
    plt.yticks(fontsize=18)
    plt.title(title, fontsize=22)
    plt.xlabel("Split configuration", fontsize=20)
    plt.ylabel("Runtime (s)", fontsize=20)
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_file, format="svg")
    plt.show()

# --- Define configs ---
configs = [
    {
        "file_path": "results/undirected_split_results_2join.csv",
        "color": "steelblue",
        "title": "Runtime vs 2-join Split Configurations (k=12)",
        "output_file": "results/runtime_vs_split_2join.svg"
    },
    {
        "file_path": "results/undirected_split_results_3join.csv",
        "color": "darkorange",
        "title": "Runtime vs 3-join Split Configurations (k=12)",
        "output_file": "results/runtime_vs_split_3join.svg"
    },
    {
        "file_path": "results/undirected_split_results_4join.csv",
        "color": "seagreen",
        "title": "Runtime vs 4-join Split Configurations (k=12)",
        "output_file": "results/runtime_vs_split_4join.svg"
    }
]

# --- Generate plots ---
for config in configs:
    plot_runtime_by_split(**config)
