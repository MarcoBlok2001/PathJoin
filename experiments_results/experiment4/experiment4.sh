#!/bin/bash
#SBATCH --job-name=pj_mean
#SBATCH --ntasks=1
#SBATCH --mem=16000mb
#SBATCH --time=7-00:00:00
#SBATCH --output=logs/%j.log
#SBATCH --partition=EPYC-Milan-serial
#SBATCH --nodelist=cn39

# Constants
NUM_GRAPHS=10
NUM_RUNS=10
PROGRAM=./main
OUTPUT_DIR=results
GRAPH_DIR=graphs
GRAPH_GEN_SCRIPT=graph_generator.py

mkdir -p "$OUTPUT_DIR"

# Experiment parameters
K_VALUES=(3 4 5 6 7 8)
MEAN_DEGREES=(2 3 4 5 6 7 8 9 10)

# CSV output
CSV="$OUTPUT_DIR/degree_results.csv"
echo "graph_type,graph_id,run,k,mean_degree,runtime_ms,cycles,paths" > "$CSV"

# ----------------------------
# Main Experiment Loop
# ----------------------------
for k in "${K_VALUES[@]}"; do
    for mean_degree in "${MEAN_DEGREES[@]}"; do
        for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
            for ((run = 0; run < NUM_RUNS; run++)); do

                for graph_type_name in normal scale_free; do

                    if [[ "$graph_type_name" == "normal" ]]; then
                        graph_type=0
                    else
                        graph_type=1
                    fi

                    GRAPH_FILE="$GRAPH_DIR/${graph_type_name}_graph_${graph_id}_m${mean_degree}.net"

                    if [[ ! -f "$GRAPH_FILE" ]]; then
                        echo "Missing file: $GRAPH_FILE" >&2
                        continue
                    fi

                    START=$(date +%s.%N)
                    OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k")
                    END=$(date +%s.%N)
                    RUNTIME=$(awk "BEGIN {print $END - $START}")

                    LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
                    CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
                    NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
                    NUM_PATHS=$((NUM_FIELDS - 1))
                    PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

                    echo "$graph_type,$graph_id,$run,$k,$mean_degree,$RUNTIME,$CYCLES,$PATH_COUNTS" >> "$CSV"

                done
            done
        done
    done
done
