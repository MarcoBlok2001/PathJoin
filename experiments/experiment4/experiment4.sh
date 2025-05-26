#!/bin/bash

# Constants
NUM_GRAPHS=10
NUM_RUNS=10
K=6
PROGRAM=../.././main
OUTPUT_DIR=results
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

# Node count
NUM_NODES_UNDIR=500

mkdir -p "$OUTPUT_DIR" "$GRAPH_DIR"

# Mean degrees to test
MEAN_DEGREES=(4 6 8 10)

# Fixed variance
FIXED_VAR=4

# CSV output
SPLIT_CSV="$OUTPUT_DIR/degree_results.csv"
echo "graph_id,run,k,mean_degree,variance,runtime_ms,cycles,paths" > "$SPLIT_CSV"

# ----------------------------
# 1. Graph Generation for each mean degree
# ----------------------------
for mean_degree in "${MEAN_DEGREES[@]}"; do
    safe_mean=${mean_degree}  # no dot so no replacement needed
    for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
        SEED=$((graph_id + 100))
        python3 "$GRAPH_GEN_SCRIPT" \
            --seed "$SEED" \
            --nodes "$NUM_NODES_UNDIR" \
            --mean "$mean_degree" \
            --var "$FIXED_VAR" \
            --out "$GRAPH_DIR/graph_${graph_id}_m${safe_mean}.net"
    done
done

# ----------------------------
# 2. Experiment Runs for K=6
# ----------------------------
for mean_degree in "${MEAN_DEGREES[@]}"; do
    for ((run = 0; run < NUM_RUNS; run++)); do
        for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
            GRAPH_FILE="$GRAPH_DIR/graph_${graph_id}_m${mean_degree}.net"

            START=$(date +%s.%N)
            OUTPUT=$($PROGRAM "$GRAPH_FILE" "$K")
            END=$(date +%s.%N)
            RUNTIME=$(echo "$END - $START" | bc)

            LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
            CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
            NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
            NUM_PATHS=$((NUM_FIELDS - 1))
            PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

            echo "$graph_id,$run,$K,$mean_degree,$FIXED_VAR,$(echo "$RUNTIME * 1000" | bc),$CYCLES,$PATH_COUNTS" >> "$SPLIT_CSV"
        done
    done
done

# ----------------------------
# 3. Cleanup
# ----------------------------
rm -rf "$GRAPH_DIR"
