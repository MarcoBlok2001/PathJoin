#include <stdlib.h>
#include "walks.h"

int MAX_WALKS = 15000000;

static void dfs(int **adj, int n, int k, int node, int depth, int *path, int **walks, int *walk_count) {
    if (*walk_count >= MAX_WALKS) {
        printf("Max walks exceeded\n");
        return;
    }

    if (depth == k) {
        // save walk
        walks[*walk_count] = malloc((k + 1) * sizeof(int));
        memcpy(walks[*walk_count], path, (k + 1) * sizeof(int));
        (*walk_count)++;
        return;
    }

    for (int neighbor = 0; neighbor < n; neighbor++) {
        if (adj[node][neighbor]) {
            // Option 1: prevent immediate backtrack
            // if(depth > 0 && neighbor == path[depth - 1]){ // prevent immediate backtracking
            //     continue;
            // }

            // Option 2: prevent revisiting any node twice.
            // prevent revisiting any node in the path
            int visited = 0;
            for (int i = 0; i <= depth; i++) {
                if (path[i] == neighbor) {
                    visited = 1;
                    break;
                }
            }
            if (visited) {
                continue;
            }

            path[depth + 1] = neighbor;
            dfs(adj, n, k, neighbor, depth + 1, path, walks, walk_count);
        }
    }
}

int** get_walks(int **adj, int n, int k, int *count) {
    int **walks = malloc(MAX_WALKS * sizeof(int *));
    *count = 0;

    int *path = malloc((k + 1) * sizeof(int));

    for (int start = 0; start < n; start++) {
        path[0] = start;
        dfs(adj, n, k, start, 0, path, walks, count);
    }

    free(path);
    return walks;
}

void print_walks(int **walks, int k, int count) {
    printf("Found %d walks:\n", count);
    for (int i = 0; i < count; i++) {
        for (int j = 0; j <= k; j++) {
            printf("%d ", walks[i][j]);
        }
        printf("\n");
    }
}

void free_walks(int **walks, int count) {
    for (int i = 0; i < count; i++) {
        free(walks[i]);
    }
    free(walks);
}
