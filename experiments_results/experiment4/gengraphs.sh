#!/bin/bash

# Constants
NUM_GRAPHS=2
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py
NUM_NODES=100

mkdir -p "$GRAPH_DIR"

# Mean degrees to test
MEAN_DEGREES=(2 3 4 5 6 7 8 9 10)

# Loop over mean degrees
for mean_degree in "${MEAN_DEGREES[@]}"; do
    for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
        SEED=$((graph_id + 100))

        # Generate normal graph
        OUTFILE="$GRAPH_DIR/normal_graph_${graph_id}_m${mean_degree}.net"
        python3 "$GRAPH_GEN_SCRIPT" \
            --seed "$SEED" \
            --nodes "$NUM_NODES" \
            --mean "$mean_degree" \
            --out "$OUTFILE"

        # Generate scale-free graph
        OUTFILE="$GRAPH_DIR/scale_free_graph_${graph_id}_m${mean_degree}.net"
        python3 "$GRAPH_GEN_SCRIPT" \
            --seed "$SEED" \
            --nodes "$NUM_NODES" \
            --mean "$mean_degree" \
            --scale_free \
            --out "$OUTFILE"
    done
done
