import argparse
import networkx as nx
import random

def generate_graph(seed, num_nodes, num_edges, directed=False):
    random.seed(seed)
    G = nx.gnm_random_graph(num_nodes, num_edges, seed=seed, directed=directed)
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
    parser.add_argument("--edges", type=int, required=True, help="Number of edges in the graph")
    parser.add_argument("--out", type=str, default="graph.net")
    parser.add_argument("--directed", action="store_true")
    args = parser.parse_args()

    G = generate_graph(args.seed, args.nodes, args.edges, directed=args.directed)
    write_pajek_net(G, args.out)
