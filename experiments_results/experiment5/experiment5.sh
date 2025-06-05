#!/bin/bash
#SBATCH --job-name=pj_2core
#SBATCH --ntasks=1
#SBATCH --mem=16000mb
#SBATCH --time=7-00:00:00
#SBATCH --output=logs/%j.log
#SBATCH --partition=EPYC-Milan-serial
#SBATCH --nodelist=cn39

# Constants
NUM_GEN_GRAPHS=10
NUM_REAL_GRAPHS=2
NUM_RUNS=10
K=14
PROGRAM=./main
OUTPUT_DIR=results
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

mkdir -p "$OUTPUT_DIR"

# CSV output
RESULT_CSV="$OUTPUT_DIR/results.csv"
echo "graph_id,run,k,2core_flag,runtime_ms,cycles,paths" > "$RESULT_CSV"

GEN_GRAPH_IDS=$(seq 0 $((NUM_GEN_GRAPHS - 1)))

REAL_GRAPH_FILES=(
    # "david_copperfield.net"
    "euroroads.net"
    "us_power_grid.net"
)

# ----------------------------
# 1. Experiment Runs for 2core={false,true} and k=3..10
# ----------------------------
for k in $(seq 3 $K); do
    for two_core in false true; do
        # Generated Graphs
        for graph_id in $GEN_GRAPH_IDS; do
            for ((run = 0; run < NUM_RUNS; run++)); do
                GRAPH_FILE="$GRAPH_DIR/graph_${graph_id}_undirected.net"

                START=$(date +%s.%N)
                OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k" -twocore $two_core)
                END=$(date +%s.%N)
                RUNTIME=$(awk "BEGIN {print $END - $START}")

                LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
                CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
                NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
                NUM_PATHS=$((NUM_FIELDS - 1))
                PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

                echo "$graph_id,$run,$k,$two_core,$RUNTIME,$CYCLES,$PATH_COUNTS" >> "$RESULT_CSV"
            done
        done

        # Real-world Graphs
        for idx in "${!REAL_GRAPH_FILES[@]}"; do
            real_graph_id=$((NUM_GEN_GRAPHS + idx))
            GRAPH_FILE="$GRAPH_DIR/${REAL_GRAPH_FILES[$idx]}"
            for ((run = 0; run < NUM_RUNS; run++)); do

                START=$(date +%s.%N)
                OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k" -twocore $two_core)
                END=$(date +%s.%N)
                RUNTIME=$(awk "BEGIN {print $END - $START}")

                LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
                CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
                NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
                NUM_PATHS=$((NUM_FIELDS - 1))
                PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

                echo "$real_graph_id,$run,$k,$two_core,$RUNTIME,$CYCLES,$PATH_COUNTS" >> "$RESULT_CSV"
            done
        done
    done
done


