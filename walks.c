// walks.c
#include <stdlib.h>
#include "walks.h"

typedef struct {
    int **walks;
    int *count;
    int capacity;
} WalksData;

static void dfs(int **adj, int n_vertices, int len, int node, int depth, int *path, WalksData *data) {
    if (*data->count >= data->capacity) {
        data->capacity *= 2;
        data->walks = realloc(data->walks, sizeof(int*) * data->capacity);

        printf("Rescaling walks to %d\n", data->capacity);
    };

    if (depth == len) {
        // if (path[0] > path[len - 1]) return;
        // save walk
        data->walks[*data->count] = malloc((len + 1) * sizeof(int));
        memcpy(data->walks[*data->count], path, (len + 1) * sizeof(int));
        (*data->count)++;
        return;
    }

    for (int neighbor = 0; neighbor < n_vertices; neighbor++) {
        if (adj[node][neighbor]) {
            // Option 1: prevent immediate backtrack
            // if(depth > 0 && neighbor == path[depth - 1]){ // prevent immediate backtracking
            //     continue;
            // }

            // Option 2: prevent revisiting any node twice. (broken)?
            // prevent revisiting any node in the path
            int visited = 0;
            for (int i = 0; i <= depth; i++) {
                if (path[i] == neighbor) {
                    if (!(i == 0 && depth + 1 == len && neighbor == path[0])) {
                        visited = 1;
                        break;
                    }
                }
            }
            if (visited) {
                continue;
            }

            path[depth + 1] = neighbor;
            dfs(adj, n_vertices, len, neighbor, depth + 1, path, data);
        }
    }
}

int** get_walks(int **adj, int n_vertices, int len, int *count) {
    WalksData *data = malloc(sizeof(WalksData));
    data->count = count;
    data->capacity = 1000;
    data->walks = malloc(data->capacity * sizeof(int *));

    int *path = malloc((len + 1) * sizeof(int));

    for (int start = 0; start < n_vertices; start++) {
        path[0] = start;
        dfs(adj, n_vertices, len, start, 0, path, data);
    }

    free(path);
    int **walks = data->walks;
    free(data);
    return walks;
}

void print_walks(int **walks, int len, int count) {
    printf("Found %d walks:\n", count);
    for (int i = 0; i < count; i++) {
        for (int j = 0; j <= len; j++) {
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
