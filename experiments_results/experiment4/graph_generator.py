import argparse
import networkx as nx
import numpy as np

def generate_graph(seed, num_nodes, mean, scale_free=False):
    np.random.seed(seed)

    if scale_free:
        m = max(1, int(mean))  # Use mean as m if given
        G = nx.barabasi_albert_graph(n=num_nodes, m=m, seed=seed)
        degrees = np.array([d for _, d in G.degree()])
    else:
        std_dev = 1.5
        degrees = np.random.normal(loc=mean, scale=std_dev, size=num_nodes)
        degrees = np.clip(np.round(degrees), 0, num_nodes - 1).astype(int)
        if np.sum(degrees) % 2 != 0:
            degrees[0] += 1
        if not nx.is_valid_degree_sequence_erdos_gallai(degrees.tolist()):
            raise ValueError("Degree sequence is not graphical. Try a different mean.")
        G = nx.configuration_model(degrees)
        G = nx.Graph(G)
        G.remove_edges_from(nx.selfloop_edges(G))
        degrees = np.array([d for _, d in G.degree()])  # update degrees

    return G, degrees


def write_pajek_net(G, output_path):
    with open(output_path, 'w') as f:
        f.write(f"*vertices {G.number_of_nodes()}\n")
        f.write(f"*edges {G.number_of_edges()}\n")
        for u, v in G.edges():
            f.write(f"{u+1} {v+1}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--seed", type=int, required=True)
    parser.add_argument("--nodes", type=int, required=True)
    parser.add_argument("--mean", type=float, required=True, help="Target mean degree")
    parser.add_argument("--scale_free", action="store_true", help="Generate a scale-free (power-law) graph instead of using normal distribution")
    parser.add_argument("--out", type=str, default="graph.net")

    args = parser.parse_args()

    G, degrees = generate_graph(args.seed, args.nodes, args.mean, args.scale_free)

    # Print actual stats
    actual_mean = np.mean(degrees)
    actual_var = np.var(degrees)
    # print(f"Actual degree mean: {actual_mean:.2f}, variance: {actual_var:.2f}")
    # print(f"Graph clustering coefficient: {nx.average_clustering(G):.4f}")

    write_pajek_net(G, args.out)
