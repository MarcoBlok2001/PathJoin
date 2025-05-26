#!/bin/bash

# Constants
NUM_GRAPHS=10
NUM_RUNS=2
PROGRAM=../.././main
OUTPUT_DIR=results
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

# Node and edge counts
NUM_NODES_UNDIR=2000
NUM_EDGES_UNDIR=3000

mkdir -p "$OUTPUT_DIR" "$GRAPH_DIR"

# CSV output
RESULT_CSV="$OUTPUT_DIR/results.csv"
echo "graph_id,run,k,2core_flag,runtime_ms,cycles,paths" > "$RESULT_CSV"

# ----------------------------
# 1. Graph Generation
# ----------------------------
for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
    SEED=$((graph_id + 100))
    python3 "$GRAPH_GEN_SCRIPT" --seed "$SEED" --nodes "$NUM_NODES_UNDIR" --edges "$NUM_EDGES_UNDIR" --out "$GRAPH_DIR/graph_${graph_id}_undirected.net"
done

# ----------------------------
# 2. Experiment Runs for 2core={false,true} and k=3..10
# ----------------------------

for two_core in false true; do
    for ((k = 3; k <= 14; k++)); do
        for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
            for ((run = 0; run < NUM_RUNS; run++)); do
                GRAPH_FILE="$GRAPH_DIR/graph_${graph_id}_undirected.net"

                START=$(date +%s.%N)
                OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k" -twocore $two_core)
                END=$(date +%s.%N)
                RUNTIME=$(echo "$END - $START" | bc)

                LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
                CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
                NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
                NUM_PATHS=$((NUM_FIELDS - 1))
                PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

                echo "$graph_id,$run,$k, $two_core,$(echo "$RUNTIME * 1000" | bc),$CYCLES,$PATH_COUNTS" >> "$RESULT_CSV"
            done
        done
    done
done

# ----------------------------
# 3. Cleanup
# ----------------------------
rm -rf "$GRAPH_DIR"
