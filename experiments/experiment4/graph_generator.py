import argparse
import networkx as nx
import numpy as np

def generate_graph(seed, num_nodes, mean, var):
    np.random.seed(seed)

    std_dev = np.sqrt(var)

    # Step 1: Sample degree sequence
    degrees = np.random.normal(loc=mean, scale=std_dev, size=num_nodes)
    degrees = np.clip(np.round(degrees), 0, num_nodes - 1).astype(int)

    # Ensure even sum
    if np.sum(degrees) % 2 != 0:
        degrees[0] += 1

    # Step 2: Check if graphical
    if not nx.is_valid_degree_sequence_erdos_gallai(degrees.tolist()):
        raise ValueError("Degree sequence is not graphical. Try different mean/variance.")

    # Step 3: Generate graph
    G = nx.configuration_model(degrees)
    G = nx.Graph(G)  # Remove parallel edges
    G.remove_edges_from(nx.selfloop_edges(G))  # Remove self-loops

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
    parser.add_argument("--var", type=float, required=True, help="Target degree variance")
    parser.add_argument("--out", type=str, default="graph.net")

    args = parser.parse_args()

    G, degrees = generate_graph(args.seed, args.nodes, args.mean, args.var)

    # Print actual stats
    actual_mean = np.mean(degrees)
    actual_var = np.var(degrees)
    print(f"Actual degree mean: {actual_mean:.2f}, variance: {actual_var:.2f}")
    print(f"Graph clustering coefficient: {nx.average_clustering(G):.4f}")

    write_pajek_net(G, args.out)
