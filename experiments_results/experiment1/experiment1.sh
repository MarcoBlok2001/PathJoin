#!/bin/bash
#SBATCH --job-name=pj_method
#SBATCH --ntasks=1
#SBATCH --mem=16000mb
#SBATCH --time=7-00:00:00
#SBATCH --output=logs/%j.log
#SBATCH --partition=EPYC
#SBATCH --nodelist=cn29

# Constants
NUM_GRAPHS=10
NUM_RUNS=10
JOIN_METHODS=(2 3 4)
K_START=4
MAX_K=16
PROGRAM=./main
OUTPUT_DIR=results
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py


mkdir -p "$OUTPUT_DIR"

# CSV files
UNDIR_CSV="$OUTPUT_DIR/undirected_results.csv"

echo "graph_id,run,k,join,runtime_ms,cycles,paths" > "$UNDIR_CSV"

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
# 2. Experiment Runs
# ----------------------------



for k in $(seq $K_START $MAX_K); do
    for j in "${JOIN_METHODS[@]}"; do
        for ((run = 0; run < NUM_RUNS; run++)); do
            for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
                GRAPH_FILE="$GRAPH_DIR/graph_${graph_id}_undirected.net"

                START=$(date +%s.%N)
                C_ARGS=$(generate_c_args $k $j)

                if [ "$directed" = true ]; then
                    OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k" -c $C_ARGS -d true)
                else
                    OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k" -c $C_ARGS)
                fi

                END=$(date +%s.%N)
                RUNTIME=$(awk "BEGIN {print $END - $START}")

                LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
                CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
                NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
                NUM_PATHS=$((NUM_FIELDS - 1))
                PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

                echo "$graph_id,$run,$k,$j,$RUNTIME,$CYCLES,$PATH_COUNTS" >> "$UNDIR_CSV"
            done
        done
    done
done


