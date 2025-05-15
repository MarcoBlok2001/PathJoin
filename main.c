// main.c
#include <stdio.h>

#include "graph_io.h"
#include "walks.h"
#include "walkjoin.h"

#define MAX_CONFIG 4

typedef struct {
    char* filename;
    int cyclesize;
    int u_flag;
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

    opts->u_flag = 0;
    opts->config_len = 0;

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing value for -u\n");
                return 0;
            }
            opts->u_flag = strcmp(argv[i + 1], "true") == 0 ? 1 :
                           strcmp(argv[i + 1], "false") == 0 ? 0 : -1;
            if (opts->u_flag == -1) {
                fprintf(stderr, "Invalid value for -u (expected true/false): %s\n", argv[i + 1]);
                return 0;
            }
            i++;
        } else if (strcmp(argv[i], "-c") == 0) {
            int j = 0;
            while (j < MAX_CONFIG && i + 1 + j < argc && argv[i + 1 + j][0] != '-') {
                opts->config[j] = atoi(argv[i + 1 + j]);
                j++;
            }
            if (j < 2) {
                fprintf(stderr, "-c flag requires at least 2 integers\n");
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

int main(int argc, char* argv[]) {
    ProgramOptions opts;
    if (!parse_arguments(argc, argv, &opts)) return 1;

    // Debug prints
    printf("filename: %s\n", opts.filename);
    printf("cyclesize: %d\n", opts.cyclesize);
    printf("u_flag: %s\n", opts.u_flag ? "true" : "false");
    printf("config (%d values):", opts.config_len);
    for (int i = 0; i < opts.config_len; i++) {
        printf(" %d", opts.config[i]);
    }
    printf("\n");


    // Open file
    FILE *file = fopen(opts.filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    int num_vertices = 0;
    int **adj = parse(file, &num_vertices);
    if (!adj) {
        perror("Error during adj matrix creation.\n");
        return 1;
    }

    // Get necessary walks
    int walk_sizes[4];
    WalkMapEntry* walks[4] = {NULL, NULL, NULL, NULL};
    int unique_count = 0;

    // Determine unique walk sizes
    for (int i = 0; i < opts.config_len; i++) {
        int found = 0;
        for (int j = 0; j < unique_count; j++) {
            if (opts.config[i] == walk_sizes[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            walk_sizes[unique_count++] = opts.config[i];
        }
    }

    // Get walks for each unique walk size
    for (int i = 0; i < unique_count; i++) {
        walks[i] = get_walks(adj, num_vertices, walk_sizes[i]);
    }


    printf("Got walks\n");

    WalkMapEntry* config_walks[4];
    for (int i = 0; i < opts.config_len; i++) {
        for (int j = 0; j < unique_count; j++) {
            if (opts.config[i] == walk_sizes[j]) {
                config_walks[i] = walks[j];
                break;
            }
        }
    }

    int cycle_count = 0;
    int **cycles = NULL;

    if (opts.config_len == 2) {
        cycles = walk_join(config_walks[0], opts.config[0],
                        config_walks[1], opts.config[1],
                        num_vertices, &cycle_count);
    } else if (opts.config_len == 3) {
        cycles = walk_join_three(config_walks[0], opts.config[0],
                                config_walks[1], opts.config[1],
                                config_walks[2], opts.config[2],
                                num_vertices, &cycle_count);
    } else if (opts.config_len == 4) {
        cycles = walk_join_four(config_walks[0], opts.config[0],
                                config_walks[1], opts.config[1],
                                config_walks[2], opts.config[2],
                                config_walks[3], opts.config[3],
                                num_vertices, &cycle_count);
    } else {
        fprintf(stderr, "Unsupported number of walks in -c. Only 2 to 4 are supported.\n");
        return 1;
    }

    printf("cycle_count: %d\n", cycle_count);

    // for (int i = 0; i < 50; i++) {

    //     for (int j = 0; j <= 4; j++) {

    //         printf("%d ", cycles[i][j]);

    //     }

    //     printf("\n");

    // }

    for (int i = 0; i < cycle_count; i++) {
        free(cycles[i]);
    }
    free(cycles);

    for (int i = 0; i < unique_count; i++) {
        free_walk_map(walks[i]);
    }

    free_adjacency_matrix(adj, num_vertices);

    return 0;
}
