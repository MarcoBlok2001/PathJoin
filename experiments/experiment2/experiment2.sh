#!/bin/bash

# Constants
NUM_GRAPHS=2
NUM_RUNS=2
K=12
PROGRAM=../.././main
OUTPUT_DIR=results
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

# Node and edge counts
NUM_NODES_UNDIR=2000
NUM_EDGES_UNDIR=3000

mkdir -p "$OUTPUT_DIR" "$GRAPH_DIR"

# CSV output
SPLIT_CSV="$OUTPUT_DIR/undirected_split_results.csv"
echo "graph_id,run,k,config_id,c_args,runtime_ms,cycles,paths" > "$SPLIT_CSV"

# ----------------------------
# 1. Graph Generation
# ----------------------------
for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
    SEED=$((graph_id + 100))
    python3 "$GRAPH_GEN_SCRIPT" --seed "$SEED" --nodes "$NUM_NODES_UNDIR" --edges "$NUM_EDGES_UNDIR" --out "$GRAPH_DIR/graph_${graph_id}_undirected.net"
done

# ----------------------------
# 2. Experiment Runs for K=6 with all 2-way splits
# ----------------------------
# Define the splits manually (must sum to $K)
declare -a SPLITS=("3 9" "4 8" "5 7" "6 6" "7 5" "8 4" "9 3")
# declare -a SPLITS=("1 2 2 4" "1 1 2 5" "1 1 1 6" "2 2 2 3")

for config_id in "${!SPLITS[@]}"; do
    split="${SPLITS[$config_id]}"
    config_index=$((config_id + 1))

    for ((run = 0; run < NUM_RUNS; run++)); do
        for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
            GRAPH_FILE="$GRAPH_DIR/graph_${graph_id}_undirected.net"

            START=$(date +%s.%N)
            OUTPUT=$($PROGRAM "$GRAPH_FILE" "$K" -c $split)
            END=$(date +%s.%N)
            RUNTIME=$(echo "$END - $START" | bc)

            LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
            CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
            NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
            NUM_PATHS=$((NUM_FIELDS - 1))
            PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

            echo "$graph_id,$run,$K,$config_index,\"$split\",$(echo "$RUNTIME * 1000" | bc),$CYCLES,$PATH_COUNTS" >> "$SPLIT_CSV"
        done
    done
done

# ----------------------------
# 3. Cleanup
# ----------------------------
rm -rf "$GRAPH_DIR"
