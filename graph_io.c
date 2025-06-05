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

int *count_degrees(int **adj, int n, int directed) {
    int *degrees = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        int degree = 0;
        for (int j = 0; j < n; j++) {
            if (adj[i][j] == 1) {
                degree++;
            }
            if (directed && adj[j][i] == 1) {
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
