// graph_io.c
#include "graph_io.h"
#include <stdlib.h>
#include <string.h>

int** parse(FILE *file, int *n, int directed) {
    char line[256];

    if (fgets(line, sizeof(line), file) == NULL || sscanf(line, "*vertices %d", n) != 1) {
        fprintf(stderr, "Invalid or missing *vertices line.\n");
        fclose(file);
        return NULL;
    }

    if (*n <= 0) {
        fprintf(stderr, "Invalid number of vertices.\n");
        fclose(file);
        return NULL;
    }

    // offset if graph vertices start at index 1.
    *n += 1;

    int **adj = malloc(*n * sizeof(int *));
    for (int i = 0; i < *n; i++) {
        adj[i] = calloc(*n, sizeof(int));
    }

    // Skip to *edges line
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "*edges", 6) == 0 || strncmp(line, "*Edges", 6) == 0) {
            break;
        }
    }

    int u, v;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d %d", &u, &v) == 2) {
            if (u < 0 || v < 0 || u >= *n || v >= *n) {
                fprintf(stderr, "Invalid edge: %d %d\n", u, v);
                continue;
            }
            adj[u][v] = 1;
            if (!directed) adj[v][u] = 1;
        }
    }

    fclose(file);
    return adj;
}

int *count_degrees(int **adj, int n) {
    int *degrees = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        int degree = 0;
        for (int j = 0; j < n; j++) {
            if (adj[i][j] == 1) {
                degree++;
            }
        }
        degrees[i] = degree;
    }
    return degrees;
}

void print_adjacency_matrix(int **adj, int n) {
    printf("Adjacency Matrix (%d x %d):\n", n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", adj[i][j]);
        }
        printf("\n");
    }
}

void free_adjacency_matrix(int **adj, int *degrees, int n) {
    for (int i = 0; i < n; i++) {
        free(adj[i]);
    }
    free(degrees);
    free(adj);
}
