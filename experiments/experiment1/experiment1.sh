#!/bin/bash

# Constants
NUM_GRAPHS=2
NUM_RUNS=2
JOIN_METHODS=(2 3 4)
MIN_K_UNDIR=4
MIN_K_DIR=4
MAX_K=10
PROGRAM=../.././main
OUTPUT_DIR=results
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

# Node and edge counts
NUM_NODES_UNDIR=2000
NUM_EDGES_UNDIR=3000
NUM_NODES_DIR=10000
NUM_EDGES_DIR=15000

mkdir -p "$OUTPUT_DIR" "$GRAPH_DIR"

# CSV files
UNDIR_CSV="$OUTPUT_DIR/undirected_results.csv"
DIR_CSV="$OUTPUT_DIR/directed_results.csv"

echo "graph_id,run,k,join,runtime_ms,cycles,paths" > "$UNDIR_CSV"
echo "graph_id,run,k,join,runtime_ms,cycles,paths" > "$DIR_CSV"

# ----------------------------
# Helper: Join Partitioning
# ----------------------------
generate_c_args() {
    local k=$1
    local j=$2
    local base=$((k / j))
    local rem=$((k % j))

    local args=()
    for ((i = 0; i < j; i++)); do
        if ((i < j - rem)); then
            args+=($base)
        else
            args+=($((base + 1)))
        fi
    done

    echo "${args[@]}"
}

# ----------------------------
# 1. Graph Generation
# ----------------------------
for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
    SEED=$((graph_id + 100))

    # Undirected
    python3 "$GRAPH_GEN_SCRIPT" --seed "$SEED" --nodes "$NUM_NODES_UNDIR" --edges "$NUM_EDGES_UNDIR" --out "$GRAPH_DIR/graph_${graph_id}_undirected.net"

    # Directed
    python3 "$GRAPH_GEN_SCRIPT" --seed "$SEED" --nodes "$NUM_NODES_DIR" --edges "$NUM_EDGES_DIR" --out "$GRAPH_DIR/graph_${graph_id}_directed.net" --directed
done

# ----------------------------
# 2. Experiment Runs
# ----------------------------
for directed in false true; do
    if [ "$directed" = true ]; then
        K_START=$MIN_K_DIR
        OUTPUT_FILE="$DIR_CSV"
        DIR_TAG="directed"
    else
        K_START=$MIN_K_UNDIR
        OUTPUT_FILE="$UNDIR_CSV"
        DIR_TAG="undirected"
    fi

    for k in $(seq $K_START $MAX_K); do
        for j in "${JOIN_METHODS[@]}"; do
            for ((run = 0; run < NUM_RUNS; run++)); do
                for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
                    GRAPH_FILE="$GRAPH_DIR/graph_${graph_id}_${DIR_TAG}.net"

                    START=$(date +%s.%N)
                    C_ARGS=$(generate_c_args $k $j)

                    if [ "$directed" = true ]; then
                        OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k" -c $C_ARGS -d true)
                    else
                        OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k" -c $C_ARGS)
                    fi

                    END=$(date +%s.%N)
                    RUNTIME=$(echo "$END - $START" | bc)

                    LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
                    CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
                    NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
                    NUM_PATHS=$((NUM_FIELDS - 1))
                    PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

                    echo "$graph_id,$run,$k,$j,$(echo "$RUNTIME * 1000" | bc),$CYCLES,$PATH_COUNTS" >> "$OUTPUT_FILE"
                done
            done
        done
    done
done

rm -rf "$GRAPH_DIR"
