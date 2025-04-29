#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Parses a Pajek-style .net graph file and constructs an adjacency matrix.
 *
 * This function reads a graph file beginning with a "*vertices <N>" line
 * and a subsequent "*edges" section containing undirected edges in the form
 * of "u v" (0-based vertex indices). It allocates and returns an N x N
 * adjacency matrix (where adj[i][j] = 1 if there is an edge, else 0).
 *
 * @param file Pointer to an already opened FILE object positioned at the start.
 * @param n    Pointer to an int where the number of vertices will be stored.
 * @return     A dynamically allocated adjacency matrix, or NULL on failure.
 *             The caller is responsible for freeing the matrix.
 */
int** parse(FILE *file, int* n) {
    char line[256];

    // Parse metadata
    if (fgets(line, sizeof(line), file) == NULL || sscanf(line, "*vertices %d", n) != 1) {
        fprintf(stderr, "Invalid or missing *vertices line.\n");
        fclose(file);
        return NULL;
    }

    // Check if vertices are plausible.
    if (n <= 0) {
        fprintf(stderr, "invalid amount of vertices. \n");
        fclose(file);
        return NULL;
    } else {
        printf("num vertices: %d\n", *n);
    }

    // Allocate adjacency matrix
    int **adj = malloc(*n * sizeof(int *));
    for (int i = 0; i < *n; i++) {
        adj[i] = calloc(*n, sizeof(int));
    }

    // Look for *edges line
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "*edges", 6) == 0 || strncmp(line, "*Edges", 6) == 0) {
            break;
        }
    }

    // Read edges
    int u, v;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d %d", &u, &v) == 2) {
            if (u < 0 || v < 0 || u >= *n || v >= *n) {
                fprintf(stderr, "Invalid edge: %d %d\n", u, v);
                continue;
            }

            adj[u][v] = 1;
            adj[v][u] = 1; // Remove if you want undirected.
        }
    }

    fclose(file);

    return adj;
}

int main(int argc, char* argv[]) {
    printf("You have entered %d arguments:\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

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

    printf("Adjacency Matrix (%d x %d):\n", num_vertices, num_vertices);
    for (int i = 0; i < num_vertices; i++) {
        for (int j = 0; j < num_vertices; j++) {
            printf("%d ", adj[i][j]);
        }
        printf("\n");
    }

    // Free memory
    for (int i = 0; i < num_vertices; i++) {
        free(adj[i]);
    }
    free(adj);
    return 0;

}
