#!/bin/bash

# Constants
NUM_GRAPHS=2
NUM_RUNS=1
K=6
PROGRAM=../.././main
OUTPUT_DIR=results
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

# Node and edge counts
NUM_NODES_UNDIR=500

mkdir -p "$OUTPUT_DIR" "$GRAPH_DIR"

# P coefficients to test
P_COEFFS=(0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9)

# CSV output
SPLIT_CSV="$OUTPUT_DIR/p_results.csv"
echo "graph_id,run,k,p,runtime_ms,cycles,paths" > "$SPLIT_CSV"

# ----------------------------
# 1. Graph Generation per P Coefficient
# ----------------------------
for p in "${P_COEFFS[@]}"; do
    safe_p=${p/./_}  # replace . with _ for filenames
    for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
        SEED=$((graph_id + 100))
        python3 "$GRAPH_GEN_SCRIPT" \
            --seed "$SEED" \
            --nodes "$NUM_NODES_UNDIR" \
            --p_coeff "$p" \
            --out "$GRAPH_DIR/graph_${graph_id}_c${safe_p}.net"
    done
done

# ----------------------------
# 2. Experiment Runs for K=6
# ----------------------------
for p in "${P_COEFFS[@]}"; do
    safe_p=${p/./_}
    for ((run = 0; run < NUM_RUNS; run++)); do
        for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
            GRAPH_FILE="$GRAPH_DIR/graph_${graph_id}_c${safe_p}.net"

            START=$(date +%s.%N)
            OUTPUT=$($PROGRAM "$GRAPH_FILE" "$K")
            END=$(date +%s.%N)
            RUNTIME=$(echo "$END - $START" | bc)

            LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
            CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
            NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
            NUM_PATHS=$((NUM_FIELDS - 1))
            PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

            echo "$graph_id,$run,$K,$p,$(echo "$RUNTIME * 1000" | bc),$CYCLES,$PATH_COUNTS" >> "$SPLIT_CSV"
        done
    done
done

# ----------------------------
# 3. Cleanup
# ----------------------------
rm -rf "$GRAPH_DIR"
