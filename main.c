// main.c
#include <stdio.h>

#include "graph_io.h"
#include "walks.h"
#include "walkwrap.h"
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

    int walk_count = 0;
    int **walks = get_walks(adj, num_vertices, 8, &walk_count);
    printf("walk_count: %d\n", walk_count);
    // print_adjacency_matrix(adj, num_vertices);
    // int walk4_count = 0;
    // int **walks4 = get_walks(adj, num_vertices, 6, &walk4_count);
    // printf("walk4_count: %d\n", walk4_count);

    // int walk6_count = 0;
    // int **walks6 = get_walks(adj, num_vertices, 6, &walk6_count);
    // printf("walk6_count: %d\n", walk6_count);

    // int walk8_count = 0;
    // int **walks8 = get_walks(adj, num_vertices, 8, &walk8_count);
    // printf("walk8_count: %d\n", walk8_count);
    // print_walks(walks, 6, walk_count);
    int cycle_count = 0;
    // int **cycles = walk_wrap(walks, 8, walk_count, num_vertices, &cycle_count);
    int **cycles = walk_join(walks, 8, walk_count, num_vertices, &cycle_count);
    // int **cycles = walk_join_three(walks, 4, walk_count, num_vertices, &cycle_count);
    // int **cycles = walk_join_four(walks, 2, walk_count, num_vertices, &cycle_count);
    // int **cycles = walk_join_mixed(walks, 5, walk_count, walks4, 6, walk4_count, num_vertices, &cycle_count);

    printf("cycle_count: %d\n", cycle_count);
    print_walks(cycles, 16, 20);
    free_walks(cycles, cycle_count);
    free_walks(walks, walk_count);
    // free_walks(walks4, walk4_count);
    // free_walks(walks6, walk6_count);
    // free_walks(walks8, walk8_count);

    free_adjacency_matrix(adj, num_vertices);

    return 0;
}
