// main.c
#include <stdio.h>

#include "graph_io.h"
#include "walks.h"
#include "walkjoin.h"


int main(int argc, char* argv[]) {
    // Check if filename is passed
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <graph_file>\n", argv[0]);
        return 1;
    }

    // Open file
    FILE *file = fopen(argv[1], "r");
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

    // int walk_count = 0;
    // int **walks = get_walks(adj, num_vertices, 3, &walk_count);
    // printf("walk_count: %d\n", walk_count);
    // print_adjacency_matrix(adj, num_vertices);
    // int walk4_count = 0;
    // int **walks4 = get_walks(adj, num_vertices, 6, &walk4_count);
    // printf("walk4_count: %d\n", walk4_count);

    // int walk6_count = 0;
    // int **walks6 = get_walks(adj, num_vertices, 6, &walk6_count);
    // printf("walk6_count: %d\n", walk6_count);

    WalkMapEntry *walks1 = get_walks(adj, num_vertices, 1);
    // WalkMapEntry *walks2 = get_walks(adj, num_vertices, 2);
    printf("Got walks\n");
    // print_walks(walks, 6, walk_count);
    int cycle_count = 0;

    // int **cycles = walk_join(walks2, 2, walks2, 2, num_vertices, &cycle_count);
    // int **cycles = walk_join_three(walks1, 1, walks1, 1, walks1, 1, num_vertices, &cycle_count);
    int **cycles = walk_join_four(walks1, 1, walks1, 1, walks1, 1, walks1, 1, num_vertices, &cycle_count);

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

    free_walk_map(walks1);
    // free_walk_map(walks2);

    free_adjacency_matrix(adj, num_vertices);

    // int cycle[5] = {8, 9, 25, 24, 8};
    // int len = 4;

    // int* testcycle = canonical_cycle(cycle, len);
    // for (int j = 0; j <= 4; j++) {

    //     printf("%d ", testcycle[j]);

    // }

    // printf("\n");

    return 0;
}
