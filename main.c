// main.c
#include <stdio.h>

#include "graph_io.h"
#include "walks.h"
#include "walkjoin.h"

#define MAX_CONFIG 4

typedef struct {
    char* filename;
    int cyclesize;
    int directed;
    int config[MAX_CONFIG];
    int config_len;
} ProgramOptions;

int parse_arguments(int argc, char* argv[], ProgramOptions* opts) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <graph_file> <cyclesize> [-u true|false] [-c int1 int2 int3 int4]\n", argv[0]);
        return 0;
    }

    opts->filename = argv[1];
    opts->cyclesize = atoi(argv[2]);
    if (opts->cyclesize <= 0) {
        fprintf(stderr, "Invalid cyclesize: %s\n", argv[2]);
        return 0;
    }

    opts->directed = 0;
    opts->config_len = 0;

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing value for -u\n");
                return 0;
            }
            opts->directed = strcmp(argv[i + 1], "true") == 0 ? 1 :
                           strcmp(argv[i + 1], "false") == 0 ? 0 : -1;
            if (opts->directed == -1) {
                fprintf(stderr, "Invalid value for -u (expected true/false): %s\n", argv[i + 1]);
                return 0;
            }
            i++;
        } else if (strcmp(argv[i], "-c") == 0) {
            int j = 0;
            int c_sum = 0;
            while (j < MAX_CONFIG && i + 1 + j < argc && argv[i + 1 + j][0] != '-') {
                opts->config[j] = atoi(argv[i + 1 + j]);
                if (opts->config[j] <= 0) {
                    fprintf(stderr, "-c flag values cannot be 0 or lower\n");
                    return 0;
                }
                c_sum += opts->config[j];
                j++;
            }
            if (j < 2) {
                fprintf(stderr, "-c flag requires at least 2 integers\n");
                return 0;
            } else if (j > 4) {
                fprintf(stderr, "-c flag can be at most 4 integers\n");
            } else if (c_sum != opts->cyclesize) {
                fprintf(stderr, "-c flag integers must sum up to desired cyclesize\n");
                return 0;
            }
            opts->config_len = j;
            i += j;
        } else {
            fprintf(stderr, "Unknown option or misplaced argument: %s\n", argv[i]);
            return 0;
        }
    }

    return 1;
}

WalkMapEntry** get_walk_configs(ProgramOptions* opts, int** adj, int num_vertices, int* unique_count_ptr) {
    int walk_sizes[MAX_CONFIG];  // Store unique walk sizes
    WalkMapEntry* walks[MAX_CONFIG] = {NULL};
    int unique_count = 0;

    // Default config if none provided
    if (opts->config_len == 0) {
        int half = opts->cyclesize / 2;
        if (opts->cyclesize % 2 == 0) {
            opts->config_len = 2;
            opts->config[0] = half;
            opts->config[1] = half;
        } else {
            opts->config_len = 2;
            opts->config[0] = half;
            opts->config[1] = half + 1;
        }
    }

    for (int i = 0; i < opts->config_len; i++) {
        int found = 0;
        for (int j = 0; j < unique_count; j++) {
            if (opts->config[i] == walk_sizes[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            walk_sizes[unique_count] = opts->config[i];
            walks[unique_count] = get_walks(adj, num_vertices, walk_sizes[unique_count]);
            unique_count++;
        }
    }

    WalkMapEntry** config_walks = malloc(sizeof(WalkMapEntry*) * opts->config_len);
    if (!config_walks) {
        fprintf(stderr, "Memory allocation failed for config_walks\n");
        return NULL;
    }

    for (int i = 0; i < opts->config_len; i++) {
        for (int j = 0; j < unique_count; j++) {
            if (opts->config[i] == walk_sizes[j]) {
                config_walks[i] = walks[j];
                break;
            }
        }
    }

    *unique_count_ptr = unique_count;  // Write back the result
    return config_walks;
}




int** run_walk_join(WalkMapEntry** config_walks, ProgramOptions* opts, int num_vertices, int *cycle_count) {
    int config_len = opts->config_len;
    int *config = opts->config;

    if (config_len == 2) {
        return walk_join(config_walks[0], config[0],
                         config_walks[1], config[1],
                         num_vertices, cycle_count);
    } else if (config_len == 3) {
        return walk_join_three(config_walks[0], config[0],
                               config_walks[1], config[1],
                               config_walks[2], config[2],
                               num_vertices, cycle_count);
    } else if (config_len == 4) {
        return walk_join_four(config_walks[0], config[0],
                              config_walks[1], config[1],
                              config_walks[2], config[2],
                              config_walks[3], config[3],
                              num_vertices, cycle_count);
    } else {
        fprintf(stderr, "Unsupported config length: %d\n", config_len);
        return NULL;
    }
}

int main(int argc, char* argv[]) {
    ProgramOptions opts;
    if (!parse_arguments(argc, argv, &opts)) return 1;


    // Open file
    FILE *file = fopen(opts.filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Parse input file into adj matrix
    int num_vertices = 0;
    int **adj = parse(file, &num_vertices, opts.directed);
    if (!adj) {
        perror("Error during adj matrix creation.\n");
        return 1;
    }

    // Get walks
    int unique_count = 0;
    WalkMapEntry **config_walks = get_walk_configs(&opts, adj, num_vertices, &unique_count);
    printf("Got walks\n");

    // Debug prints
    printf("filename: %s\n", opts.filename);
    printf("cyclesize: %d\n", opts.cyclesize);
    printf("u_flag: %s\n", opts.directed ? "true" : "false");
    printf("config (%d values): [ ", opts.config_len);
    for (int i = 0; i < opts.config_len; i++) {
        printf("%d ", opts.config[i]);
    }
    printf("]\n");

    // Get cycles
    int cycle_count = 0;
    int **cycles = run_walk_join(config_walks, &opts, num_vertices, &cycle_count);

    printf("cycle_count: %d\n", cycle_count);





    // Free cycles
    for (int i = 0; i < cycle_count; i++) {
        free(cycles[i]);
    }
    free(cycles);

    // free walks and walk config.
    for (int i = 0; i < unique_count; i++) {
        free_walk_map(config_walks[i]);
    }
    free(config_walks);

    // free adj matrix
    free_adjacency_matrix(adj, num_vertices);

    return 0;
}
