#!/bin/bash
#SBATCH --job-name=pj_split
#SBATCH --ntasks=1
#SBATCH --mem=16000mb
#SBATCH --time=7-00:00:00
#SBATCH --output=logs/%j.log
#SBATCH --partition=EPYC-Milan-serial
#SBATCH --nodelist=cn39

# Constants
NUM_GRAPHS=10
NUM_RUNS=10
K=12
PROGRAM=./main
OUTPUT_DIR=results
GRAPH_DIR=graphs

mkdir -p "$OUTPUT_DIR"

# Define split sets k = 8
# declare -a SPLIT_SETS_NAMES=("2join" "3join" "4join")
# declare -a SPLIT_SETS_2JOIN=("2 6" "3 5" "4 4" "5 3" "6 2")
# declare -a SPLIT_SETS_3JOIN=("4 3 1" "3 3 2" "3 2 3" "2 3 3" "1 3 4")
# declare -a SPLIT_SETS_4JOIN=("3 3 1 1" "3 2 2 1" "2 2 2 2" "1 2 2 3" "1 1 3 3")

# Define split sets k = 12
declare -a SPLIT_SETS_NAMES=("2join" "3join" "4join")
declare -a SPLIT_SETS_2JOIN=("4 8" "5 7" "6 6" "7 5" "8 4")
declare -a SPLIT_SETS_3JOIN=("6 5 1" "5 5 2" "4 4 4" "2 5 5" "1 5 6")
declare -a SPLIT_SETS_4JOIN=("5 5 1 1" "4 4 2 2" "3 3 3 3" "2 2 4 4" "1 1 5 5")

# Loop over split set names
for split_set_name in "${SPLIT_SETS_NAMES[@]}"; do
    csv_file="$OUTPUT_DIR/undirected_split_results_${split_set_name}.csv"
    echo "graph_id,run,k,config_id,c_args,runtime_ms,cycles,paths" > "$csv_file"

    case $split_set_name in
        "2join") splits=("${SPLIT_SETS_2JOIN[@]}") ;;
        "3join") splits=("${SPLIT_SETS_3JOIN[@]}") ;;
        "4join") splits=("${SPLIT_SETS_4JOIN[@]}") ;;
        *) echo "Unknown split set: $split_set_name"; exit 1 ;;
    esac

    for config_id in "${!splits[@]}"; do
        split="${splits[$config_id]}"
        config_index=$((config_id + 1))

        for ((run = 0; run < NUM_RUNS; run++)); do
            for ((graph_id = 0; graph_id < NUM_GRAPHS; graph_id++)); do
                GRAPH_FILE="$GRAPH_DIR/graph_${graph_id}_undirected.net"

                START=$(date +%s.%N)
                OUTPUT=$($PROGRAM "$GRAPH_FILE" "$K" -c $split)
                END=$(date +%s.%N)
                RUNTIME=$(awk "BEGIN {print $END - $START}")

                LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
                CYCLES=$(echo "$LAST_LINE" | awk -F',' '{print $NF}')
                NUM_FIELDS=$(echo "$LAST_LINE" | awk -F',' '{print NF}')
                NUM_PATHS=$((NUM_FIELDS - 1))
                PATH_COUNTS=$(echo "$LAST_LINE" | cut -d',' -f1-$NUM_PATHS | tr ',' '|')

                echo "$graph_id,$run,$K,$config_index,\"$split\",$RUNTIME,$CYCLES,$PATH_COUNTS" >> "$csv_file"
            done
        done
    done
done
