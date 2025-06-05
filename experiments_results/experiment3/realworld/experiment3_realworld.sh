#!/bin/bash
#SBATCH --job-name=pathjoin_real
#SBATCH --ntasks=1
#SBATCH --mem=16000mb
#SBATCH --time=7-00:00:00
#SBATCH --output=logs/%j.log
#SBATCH --partition=EPYC
#SBATCH --nodelist=cn29

# Constants
NUM_RUNS=10
K_START=3
MAX_K=20
PROGRAM=../../.././main
OUTPUT_DIR=results
GRAPH_DIR=graphs

mkdir -p "$OUTPUT_DIR"

# Output CSV
RESULTS_CSV="$OUTPUT_DIR/realworld_results.csv"
echo "config_id,run,k,runtime_ms,cycles,paths" > "$RESULTS_CSV"

# Graph files and config IDs
GRAPH_FILES=(
    "david_copperfield.net"
    "euroroads.net"
    "hamsterster_households.net"
    "human_proteins_figeys.net"
    "us_power_grid.net"
)

# Run experiments
for k in $(seq $K_START $MAX_K); do
    for ((config_id = 0; config_id < ${#GRAPH_FILES[@]}; config_id++)); do
        if [[ $k -ge 6 && ( $config_id -eq 2 ) ]]; then
            continue
        elif [[ $k -ge 9 && ( $config_id -eq 0 ) ]]; then
            continue
        elif [[ $k -ge 13 && ( $config_id -eq 4 ) ]]; then
            continue
        fi

        graph_file="${GRAPH_FILES[$config_id]}"
        GRAPH_PATH="$GRAPH_DIR/$graph_file"

        # Determine if directed
        if [[ "$graph_file" == "human_proteins_figeys.net" ]]; then
            EXTRA_ARGS="-d true"
        else
            EXTRA_ARGS=""
        fi

        for ((run = 0; run < NUM_RUNS; run++)); do
            START=$(date +%s.%N)

            OUTPUT=$($PROGRAM "$GRAPH_PATH" "$k" $EXTRA_ARGS)

            END=$(date +%s.%N)
            RUNTIME=$(awk "BEGIN {print $END - $START}")

            LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
            CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
            NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
            NUM_PATHS=$((NUM_FIELDS - 1))
            PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

            echo "$config_id,$run,$k,$RUNTIME,$CYCLES,$PATH_COUNTS" >> "$RESULTS_CSV"
        done
    done
done
