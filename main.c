// main.c
#include <stdio.h>

#include "graph_io.h"
#include "pre_processing.h"
#include "paths.h"
#include "pathjoin.h"

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
        fprintf(stderr, "Usage: %s <graph_file> <cyclesize> [-d true|false] [-c int1 int2 int3 int4]\n", argv[0]);
        return 0;
    }

    opts->filename = argv[1];
    opts->cyclesize = atoi(argv[2]);
    if (opts->cyclesize < 2) {
        fprintf(stderr, "Invalid cyclesize: %s. Must be of value 3 or bigger.\n", argv[2]);
        return 0;
    }

    opts->directed = 0;
    opts->config_len = 0;

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing value for -d\n");
                return 0;
            }
            opts->directed = strcmp(argv[i + 1], "true") == 0 ? 1 :
                           strcmp(argv[i + 1], "false") == 0 ? 0 : -1;
            if (opts->directed == -1) {
                fprintf(stderr, "Invalid value for -d (expected true/false): %s\n", argv[i + 1]);
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

PathMapEntry** get_path_configs(ProgramOptions* opts, int** adj, int* degrees, int num_vertices, int* unique_count_ptr, PathMapEntry*** unique_paths) {
    int path_sizes[MAX_CONFIG];  // Store unique path sizes
    PathMapEntry* paths[MAX_CONFIG] = {NULL};
    int unique_count = 0;

    // Default config if none provided
    if (opts->config_len == 0) {
        int half = opts->cyclesize / 2;
        opts->config_len = 2;
        opts->config[0] = half;

        if (opts->cyclesize % 2 == 0) {
            opts->config[1] = half;
        } else {
            opts->config[1] = half + 1;
        }
    }

    for (int i = 0; i < opts->config_len; i++) {
        int found = 0;
        for (int j = 0; j < unique_count; j++) {
            if (opts->config[i] == path_sizes[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            path_sizes[unique_count] = opts->config[i];
            paths[unique_count] = get_paths(adj, degrees, num_vertices, path_sizes[unique_count]);
            unique_count++;
        }
    }

    PathMapEntry** config_paths = malloc(sizeof(PathMapEntry*) * opts->config_len);
    if (!config_paths) {
        fprintf(stderr, "Memory allocation failed for config_paths\n");
        return NULL;
    }

    *unique_paths = malloc(sizeof(PathMapEntry*) * MAX_CONFIG);
    if (!(*unique_paths)) {
        fprintf(stderr, "Memory allocation failed for unique_paths\n");
        return NULL;
    }



    for (int i = 0; i < opts->config_len; i++) {
        for (int j = 0; j < unique_count; j++) {
            if (opts->config[i] == path_sizes[j]) {
                config_paths[i] = paths[j];
                break;
            }
        }
    }

    for (int i = 0; i < unique_count; i++) {
        (*unique_paths)[i] = paths[i];
    }

    *unique_count_ptr = unique_count;  // Write back the result
    return config_paths;
}


int** run_path_join(PathMapEntry** config_paths, ProgramOptions* opts, int num_vertices, int *cycle_count) {
    int config_len = opts->config_len;
    int *config = opts->config;

    if (config_len == 2) {
        return path_join(config_paths[0], config[0],
                         config_paths[1], config[1],
                         num_vertices, cycle_count);
    } else if (config_len == 3) {
        return path_join_three(config_paths[0], config[0],
                               config_paths[1], config[1],
                               config_paths[2], config[2],
                               num_vertices, cycle_count);
    } else if (config_len == 4) {
        return path_join_four(config_paths[0], config[0],
                              config_paths[1], config[1],
                              config_paths[2], config[2],
                              config_paths[3], config[3],
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
    int *degrees = count_degrees(adj, num_vertices, opts.directed);

    // 2-core optimization
    twocores(adj, degrees, num_vertices, opts.directed);
    
    // Get paths
    int unique_count = 0;
    PathMapEntry **unique_paths = NULL;
    PathMapEntry **config_paths = get_path_configs(&opts, adj, degrees, num_vertices, &unique_count, &unique_paths);
    printf("Got paths\n");

    // Debug prints
    printf("filename: %s\n", opts.filename);
    printf("cyclesize: %d\n", opts.cyclesize);
    printf("d_flag: %s\n", opts.directed ? "true" : "false");
    printf("config (%d values): [", opts.config_len);
    for (int i = 0; i < opts.config_len; i++) {
        printf(" - %d", opts.config[i]);
    }
    printf(" - ]\n");

    // Get cycles
    int cycle_count = 0;
    int **cycles = run_path_join(config_paths, &opts, num_vertices, &cycle_count);

    printf("cycle_count: %d\n", cycle_count);

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j <= opts.cyclesize; j++) {
            printf("%d ", cycles[i][j]);
        }
        printf("\n");
    }

    // Free cycles
    for (int i = 0; i < cycle_count; i++) {
        free(cycles[i]);
    }
    free(cycles);

    // free paths and path config.
    for (int i = 0; i < unique_count; i++) {
        free_path_map(unique_paths[i]);
    }
    free(config_paths);

    // free adj matrix
    free_adjacency_matrix(adj, degrees, num_vertices);

    return 0;
}
