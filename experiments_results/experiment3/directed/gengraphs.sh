#!/bin/bash

# Constants
NUM_GRAPHS=10

GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

mkdir -p "$GRAPH_DIR"


# ----------------------------
# 1. Configuration of Graphs
# ----------------------------
# Format: "nodes,edges"
# CONFIGS=(
#     "3000,4500"
#     "6000,9000"
#     "12000,18000"
# )

CONFIGS=(
    "1000,2000"
    "1000,2500"
    "1000,3000"

    "2000,4000"
    "3000,6000"
)

# ----------------------------
# 2. Graph Generation
# ----------------------------
for config_index in "${!CONFIGS[@]}"; do
    CONFIG="${CONFIGS[$config_index]}"
    IFS=',' read -r NUM_NODES NUM_EDGES <<< "$CONFIG"

    for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
        SEED=$((config_index * 100 + graph_id))  # Unique seed per graph/config
        OUT_FILE="$GRAPH_DIR/graph_c${config_index}_g${graph_id}.net"

        python3 "$GRAPH_GEN_SCRIPT" \
            --seed "$SEED" \
            --nodes "$NUM_NODES" \
            --edges "$NUM_EDGES" \
            --out "$OUT_FILE" \
            --directed
    done
done

