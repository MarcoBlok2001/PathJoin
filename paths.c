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

// Global count of found paths.
int COUNT = 0;

// Adds a new path to the hash map keyed by start and end vertices.
// Resizes the path list array dynamically if needed.
void add_path_to_map(PathMapEntry **map, int *path, int k) {
    PathKey key = {path[0], path[k]};
    PathMapEntry *entry = NULL;

    // Find existing entry by key
    HASH_FIND(hh, *map, &key, sizeof(PathKey), entry);

    if (!entry) {
        // Create new entry if not found
        entry = malloc(sizeof(PathMapEntry));
        entry->key = key;
        entry->count = 0;
        entry->capacity = 4;
        entry->path_list = malloc(sizeof(int*) * entry->capacity);
        HASH_ADD(hh, *map, key, sizeof(PathKey), entry);
    }

    // Resize path_list if needed
    if (entry->count == entry->capacity) {
        entry->capacity *= 2;
        entry->path_list = realloc(entry->path_list, sizeof(int*) * entry->capacity);
    }

    // Add new path pointer
    entry->path_list[entry->count++] = path;
}

// Recursive DFS to find all simple paths of length len starting from node.
// Avoids revisiting nodes already in path, adds completed paths to map.
static void dfs(int **adj, int n_vertices, int len, int node, int depth, int *path, PathMapEntry **map) {
    if (depth == len) {
        // Path complete, copy and add to map
        int *path_copy = malloc((len + 1) * sizeof(int));
        memcpy(path_copy, path, (len + 1) * sizeof(int));
        add_path_to_map(map, path_copy, len);
        COUNT++;
        return;
    }

    // Explore neighbors
    for (int neighbor = 0; neighbor < n_vertices; neighbor++) {
        if (adj[node][neighbor]) {
            // Check if neighbor already visited in path to avoid cycles
            int visited = 0;
            for (int i = 0; i <= depth; i++) {
                if (path[i] == neighbor) {
                    visited = 1;
                    break;
                }
            }
            if (visited) continue;

            // Continue DFS with neighbor
            path[depth + 1] = neighbor;
            dfs(adj, n_vertices, len, neighbor, depth + 1, path, map);
        }
    }
}

// Finds all simple paths of length len in the graph.
// Skips isolated vertices.
// Returns a hash map of paths grouped by their start and end vertices.
PathMapEntry* get_paths(int **adj, int *degrees, int n_vertices, int len, int *path_count) {
    PathMapEntry *map = NULL;
    int *path = malloc((len + 1) * sizeof(int));

    for (int start = 0; start < n_vertices; start++) {
        if (degrees[start] == 0) continue; // Skip isolated vertices
        path[0] = start;
        dfs(adj, n_vertices, len, start, 0, path, &map);
    }

    free(path);
    *path_count = COUNT;

    // reset global COUNT
    COUNT = 0;

    return map;
}

// Prints all stored paths in the hash map.
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

// Frees all allocated memory for the path map and contained paths.
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
