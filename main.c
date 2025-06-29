/*
 * MIT License
 *
 * Copyright (c) 2025 Marco Blok
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// main.c
#include <stdio.h>

#include "graph_io.h"
#include "pre_processing.h"
#include "paths.h"
#include "pathjoin.h"

#define MAX_CONFIG 4

typedef struct {
    char* filename;
    char* outfilename;
    int cyclesize;
    int directed;
    int twocore;
    int verbose;
    int config[MAX_CONFIG];
    int config_len;
} ProgramOptions;

// Parses command line arguments into ProgramOptions struct
int parse_arguments(int argc, char* argv[], ProgramOptions* opts) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <graph_file> <cyclesize> [-d true|false] [-v true|false] [-c int1 int2 int3 int4]\n", argv[0]);
        return 0;
    }

    // Required arguments
    opts->filename = argv[1];
    opts->cyclesize = atoi(argv[2]);
    if (opts->cyclesize < 2) {
        fprintf(stderr, "Invalid cyclesize: %s. Must be of value 3 or bigger.\n", argv[2]);
        return 0;
    }

    opts->directed = 0;
    opts->twocore = 1;
    opts->verbose = 0;
    opts->config_len = 0;
    opts->outfilename = NULL;

    // Optional arguments
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            // Directed flag
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
        } else if (strcmp(argv[i], "-v") == 0) {
            // Verbose output flag
            opts->verbose = 1;
        } else if (strcmp(argv[i], "-twocore") == 0) {
            // Two-core optimisation
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing value for -d\n");
                return 0;
            }
            opts->twocore = strcmp(argv[i + 1], "true") == 0 ? 1 :
                           strcmp(argv[i + 1], "false") == 0 ? 0 : -1;
            if (opts->twocore == -1) {
                fprintf(stderr, "Invalid value for -d (expected true/false): %s\n", argv[i + 1]);
                return 0;
            }
            i++;
        } else if (strcmp(argv[i], "-c") == 0) {
            // Path configuration values
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
        } else if (strcmp(argv[i], "-o") == 0) {
            // Output file name
            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                // Generate default output filename
                const char* input = opts->filename;
                const char* suffix = "_output.txt";
                const char* last_slash = strrchr(input, '/');
                const char* base = last_slash ? last_slash + 1 : input;
                const char* dot = strrchr(base, '.');

                size_t base_len = dot ? (size_t)(dot - input) : strlen(input);
                size_t total_len = base_len + strlen(suffix) + 1;

                opts->outfilename = (char*)malloc(total_len);
                if (!opts->outfilename) {
                    fprintf(stderr, "Memory allocation failed for output filename\n");
                    return 0;
                }

                snprintf(opts->outfilename, total_len, "%.*s%s", (int)base_len, input, suffix);
            } else {
                // Use user-provided output filename
                opts->outfilename = strdup(argv[i + 1]);
                if (!opts->outfilename) {
                    fprintf(stderr, "Memory allocation failed for output filename\n");
                    return 0;
                }
                i++;
            }
        } else {
            // Unknown flag
            fprintf(stderr, "Unknown option or misplaced argument: %s\n", argv[i]);
            return 0;
        }
    }

    return 1;
}

// Writes found cycles to a file
void write_cycles_to_file(const char* filename, CycleSetEntry* cycles, int cycle_count, int cyclesize) {
    FILE* out = fopen(filename, "w");
    if (!out) {
        fprintf(stderr, "Error: Could not open output file '%s' for writing.\n", filename);
        return;
    }

    fprintf(out, "cycle_count: %d\n", cycle_count);

    CycleSetEntry *entry, *tmp;
    HASH_ITER(hh, cycles, entry, tmp) {
        for (int j = 0; j <= cyclesize; j++) {
            fprintf(out, "%d ", entry->cycle[j]);
        }
        fprintf(out, "\n");
    }

    fclose(out);
}

// Prepares path structures used for cycle construction based on config
PathMapEntry** get_path_configs(ProgramOptions* opts, int** adj, int* degrees, int num_vertices, int* unique_count_ptr, PathMapEntry*** unique_paths) {
    int path_sizes[MAX_CONFIG];
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

    // Identify unique path sizes to avoid recomputation
    for (int i = 0; i < opts->config_len; i++) {
        int found = 0;
        for (int j = 0; j < unique_count; j++) {
            if (opts->config[i] == path_sizes[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            int path_count = 0;
            path_sizes[unique_count] = opts->config[i];
            paths[unique_count] = get_paths(adj, degrees, num_vertices, path_sizes[unique_count], &path_count);

            if (opts->verbose) {
                printf("Paths of length %d found: %d\n", path_sizes[unique_count], path_count);
            } else {
                printf("%d, ", path_count);
            }

            unique_count++;
        }
    }

    // Map config to corresponding path entries
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

// Calls appropriate path joining function based on config length
CycleSetEntry* run_path_join(PathMapEntry** config_paths, ProgramOptions* opts, int num_vertices, int *cycle_count) {
    int config_len = opts->config_len;
    int verbose = opts->verbose;
    int *config = opts->config;

    if (config_len == 2) {
        return path_join(config_paths[0], config[0],
                         config_paths[1], config[1],
                         num_vertices, cycle_count, verbose);
    } else if (config_len == 3) {
        return path_join_three(config_paths[0], config[0],
                               config_paths[1], config[1],
                               config_paths[2], config[2],
                               num_vertices, cycle_count, verbose);
    } else if (config_len == 4) {
        return path_join_four(config_paths[0], config[0],
                              config_paths[1], config[1],
                              config_paths[2], config[2],
                              config_paths[3], config[3],
                              num_vertices, cycle_count, verbose);
    } else {
        fprintf(stderr, "Unsupported config length: %d\n", config_len);
        return NULL;
    }
}

// Entry point of the program
int main(int argc, char* argv[]) {
    ProgramOptions opts;
    if (!parse_arguments(argc, argv, &opts)) return 1;


    // Load graph file
    FILE *file = fopen(opts.filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Parse graph into adjacency matrix
    int num_vertices = 0;
    int **adj = parse(file, &num_vertices, opts.directed);
    if (!adj) {
        perror("Error during adj matrix creation.\n");
        return 1;
    }

    // Compute vertex degrees
    int *degrees = count_degrees(adj, num_vertices, opts.directed);

    // Optional two-core optimisation
    if (opts.twocore) twocores(adj, degrees, num_vertices, opts.directed);

    // Get paths
    int unique_count = 0;
    PathMapEntry **unique_paths = NULL;
    PathMapEntry **config_paths = get_path_configs(&opts, adj, degrees, num_vertices, &unique_count, &unique_paths);


    // Verbose output
    if (opts.verbose) {
        printf("filename: %s\n", opts.filename);
        printf("cyclesize: %d\n", opts.cyclesize);
        printf("directed: %s\n", opts.directed ? "true" : "false");
        printf("config (%d-join): [", opts.config_len);
        for (int i = 0; i < opts.config_len; i++) {
            printf(" - %d", opts.config[i]);
        }
        printf(" - ]\n");
    }

    // Find cycles via PathJoin
    int cycle_count = 0;
    CycleSetEntry*cycles = run_path_join(config_paths, &opts, num_vertices, &cycle_count);

    if (!opts.verbose) {
        printf("%d\n", cycle_count);
    } else {
        printf("\nCycles found: %d\n", cycle_count);
    }

    // Write cycles to output file if specified
    if (opts.outfilename != NULL) {
        write_cycles_to_file(opts.outfilename, cycles, cycle_count, opts.cyclesize);
        if (opts.verbose) {
            printf("Cycles written to output file: %s\n", opts.outfilename);
        }
    }

    // Cleanup: free memory for cycles
    CycleSetEntry *centry, *tmp;
    HASH_ITER(hh, cycles, centry, tmp) {
        HASH_DEL(cycles, centry);
        free(centry->cycle);
        free(centry);
    }

    // Cleanup: free paths and path config.
    for (int i = 0; i < unique_count; i++) {
        free_path_map(unique_paths[i]);
    }
    free(config_paths);
    free(unique_paths);

    // Cleanup: free adjacency matrix
    free_adjacency_matrix(adj, degrees, num_vertices);

    // Cleanup: free output filename memory
    if (opts.outfilename) {
        free(opts.outfilename);
    }

    return 0;
}
