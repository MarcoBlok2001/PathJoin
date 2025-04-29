// main.c
#include <stdio.h>

#include "graph_io.h"


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

    print_adjacency_matrix(adj, num_vertices);
    free_adjacency_matrix(adj, num_vertices);

    return 0;
}
