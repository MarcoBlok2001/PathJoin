#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    char line[256];
    int num_vertices = 0;

    // Parse metadata
    if (fgets(line, sizeof(line), file) == NULL || sscanf(line, "*vertices %d", &num_vertices) != 1) {
        fprintf(stderr, "Invalid or missing *vertices line.\n");
        fclose(file);
        return 1;
    }

    // Check if vertices are plausible.
    if (num_vertices <= 0) {
        fprintf(stderr, "invalid amount of vertices. \n");
        fclose(file);
        return 1;
    } else {
        printf("num vertices: %d\n", num_vertices);
    }

    // Allocate adjacency matrix
    int **adj = malloc(num_vertices * sizeof(int *));
    for (int i = 0; i < num_vertices; i++) {
        adj[i] = calloc(num_vertices, sizeof(int));
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
            if (u < 0 || v < 0 || u >= num_vertices || v >= num_vertices) {
                fprintf(stderr, "Invalid edge: %d %d\n", u, v);
                continue;
            }

            adj[u][v] = 1;
            adj[v][u] = 1;
        }
    }

    fclose(file);

    // Print adjacency matrix
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
