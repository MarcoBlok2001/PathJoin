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

// paths.c
#include <stdlib.h>
#include "paths.h"
#include "uthash.h"

int COUNT = 0;

void add_path_to_map(PathMapEntry **map, int *path, int k) {
    PathKey key = {path[0], path[k]};
    PathMapEntry *entry = NULL;
    HASH_FIND(hh, *map, &key, sizeof(PathKey), entry);

    if (!entry) {
        entry = malloc(sizeof(PathMapEntry));
        entry->key = key;
        entry->count = 0;
        entry->capacity = 4;
        entry->path_list = malloc(sizeof(int*) * entry->capacity);
        HASH_ADD(hh, *map, key, sizeof(PathKey), entry);
    }

    if (entry->count == entry->capacity) {
        entry->capacity *= 2;
        entry->path_list = realloc(entry->path_list, sizeof(int*) * entry->capacity);
    }

    entry->path_list[entry->count++] = path;
}

static void dfs(int **adj, int n_vertices, int len, int node, int depth, int *path, PathMapEntry **map) {
    if (depth == len) {
        int *path_copy = malloc((len + 1) * sizeof(int));
        memcpy(path_copy, path, (len + 1) * sizeof(int));
        add_path_to_map(map, path_copy, len);
        COUNT++;
        return;
    }

    for (int neighbor = 0; neighbor < n_vertices; neighbor++) {
        if (adj[node][neighbor]) {
            int visited = 0;
            for (int i = 0; i <= depth; i++) {
                if (path[i] == neighbor) {
                    visited = 1;
                    break;
                }
            }
            if (visited) continue;

            path[depth + 1] = neighbor;
            dfs(adj, n_vertices, len, neighbor, depth + 1, path, map);
        }
    }
}


PathMapEntry* get_paths(int **adj, int *degrees, int n_vertices, int len) {
    PathMapEntry *map = NULL;
    int *path = malloc((len + 1) * sizeof(int));

    for (int start = 0; start < n_vertices; start++) {
        if (degrees[start] == 0) continue;
        path[0] = start;
        dfs(adj, n_vertices, len, start, 0, path, &map);
    }

    free(path);
    printf("%d, ", COUNT);
    // printf("%d-paths:%d\n", len, COUNT);
    return map;
}

void print_path_map(PathMapEntry *map, int len) {
    PathMapEntry *entry, *tmp;
    HASH_ITER(hh, map, entry, tmp) {
        printf("From %d to %d:\n", entry->key.start, entry->key.end);
        for (int i = 0; i < entry->count; i++) {
            for (int j = 0; j <= len; j++) {
                printf("%d ", entry->path_list[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void free_path_map(PathMapEntry *map) {
    PathMapEntry *entry, *tmp;
    HASH_ITER(hh, map, entry, tmp) {
        for (int i = 0; i < entry->count; i++) {
            free(entry->path_list[i]);
        }
        free(entry->path_list);
        HASH_DEL(map, entry);
        free(entry);
    }
}
