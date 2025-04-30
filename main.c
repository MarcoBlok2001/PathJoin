// main.c
#include <stdio.h>

#include "graph_io.h"
#include "walks.h"
#include "walkwrap.h"


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

    // print_adjacency_matrix(adj, num_vertices);
    int walk_count = 0;
    int **walks = get_walks(adj, num_vertices, 8, &walk_count);
    printf("walk_count: %d\n", walk_count);
    // print_walks(walks, 6, walk_count);
    int cycle_count = 0;
    int **cycles = walk_wrap(walks, 8, walk_count, num_vertices, &cycle_count);

    printf("cycle_count: %d\n", cycle_count);
    // print_walks(cycles, 4, cycle_count);
    free_walks(cycles, cycle_count);
    free_walks(walks, walk_count);
    free_adjacency_matrix(adj, num_vertices);

    return 0;
}
