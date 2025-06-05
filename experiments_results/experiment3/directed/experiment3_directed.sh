#!/bin/bash
#SBATCH --job-name=pathjoin_dir
#SBATCH --ntasks=1
#SBATCH --mem=16000mb
#SBATCH --time=7-00:00:00
#SBATCH --output=logs/%j.log
#SBATCH --partition=EPYC
#SBATCH --nodelist=cn29

# Constants
NUM_GRAPHS=10
NUM_RUNS=10
K_START=2
MAX_K=18
PROGRAM=./main
OUTPUT_DIR=results
GRAPH_DIR=graphs

mkdir -p "$OUTPUT_DIR"

# CSV files
DIR_CSV="$OUTPUT_DIR/directed_results.csv"

echo "config_id,graph_id,run,k,runtime_ms,cycles,paths" > "$DIR_CSV"

# ----------------------------
# Graph Configurations
# ----------------------------
CONFIGS=(
    "1000,2000"
    "1000,2500"
    "1000,3000"

    "2000,4000"
    "3000,6000"
)



# ----------------------------
# Experiment Runs
# ----------------------------
for k in $(seq $K_START $MAX_K); do
    for config_index in "${!CONFIGS[@]}"; do
        for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
            GRAPH_FILE="$GRAPH_DIR/graph_c${config_index}_g${graph_id}.net"

            for ((run = 0; run < NUM_RUNS; run++)); do

                START=$(date +%s.%N)

                OUTPUT=$($PROGRAM "$GRAPH_FILE" "$k" -d true)

                END=$(date +%s.%N)
                RUNTIME=$(awk "BEGIN {print $END - $START}")

                LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
                CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
                NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
                NUM_PATHS=$((NUM_FIELDS - 1))
                PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

                echo "$config_index,$graph_id,$run,$k,$RUNTIME,$CYCLES,$PATH_COUNTS" >> "$DIR_CSV"
            done
        done
    done
done

