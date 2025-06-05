#!/bin/bash

# Constants
NUM_GRAPHS=10

GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

# Node and edge counts
# NUM_NODES_UNDIR=2000
# NUM_EDGES_UNDIR=3000

NUM_NODES_UNDIR=500
NUM_EDGES_UNDIR=750

mkdir -p "$GRAPH_DIR"



# ----------------------------
# 1. Graph Generation
# ----------------------------
for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
    SEED=$((graph_id + 100))
    python3 "$GRAPH_GEN_SCRIPT" --seed "$SEED" --nodes "$NUM_NODES_UNDIR" --edges "$NUM_EDGES_UNDIR" --out "$GRAPH_DIR/graph_${graph_id}_undirected.net"
done

