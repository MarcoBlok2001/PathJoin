import argparse
import networkx as nx
import random

def generate_graph(seed, num_nodes, p_coeff=None):
    random.seed(seed)
    # Estimate m based on desired number of edges
    m = 4
    G = nx.powerlaw_cluster_graph(num_nodes, m, p_coeff, seed=seed)

    return G

def write_pajek_net(G, output_path):
    with open(output_path, 'w') as f:
        # Header: vertices
        f.write(f"*vertices {G.number_of_nodes()}\n")
        f.write(f"*edges {G.number_of_edges()}\n")
        for u, v in G.edges():
            # Pajek is 1-based; networkx uses 0-based
            f.write(f"{u+1} {v+1}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--seed", type=int, required=True)
    parser.add_argument("--nodes", type=int, required=True, help="Number of nodes in the graph")
    parser.add_argument("--out", type=str, default="graph.net")
    parser.add_argument("--p_coeff", type=float, default=None, help="P coefficient (0-1)")

    args = parser.parse_args()

    G = generate_graph(args.seed, args.nodes, p_coeff=args.p_coeff)

    print(f"Final clustering coefficient: ({args.p_coeff}), {nx.average_clustering(G):.4f}")

    write_pajek_net(G, args.out)
