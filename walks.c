// walks.c
#include <stdlib.h>
#include "walks.h"
#include "uthash.h"

void add_walk_to_map(WalkMapEntry **map, int *walk, int k) {
    WalkKey key = {walk[0], walk[k]};
    WalkMapEntry *entry = NULL;
    HASH_FIND(hh, *map, &key, sizeof(WalkKey), entry);

    if (!entry) {
        entry = malloc(sizeof(WalkMapEntry));
        entry->key = key;
        entry->count = 0;
        entry->capacity = 4;
        entry->walk_list = malloc(sizeof(int*) * entry->capacity);
        HASH_ADD(hh, *map, key, sizeof(WalkKey), entry);
    }

    if (entry->count == entry->capacity) {
        entry->capacity *= 2;
        entry->walk_list = realloc(entry->walk_list, sizeof(int*) * entry->capacity);
    }

    entry->walk_list[entry->count++] = walk;
}

static void dfs(int **adj, int n_vertices, int len, int node, int depth, int *path, WalkMapEntry **map) {
    if (depth == len) {
        int *walk = malloc((len + 1) * sizeof(int));
        memcpy(walk, path, (len + 1) * sizeof(int));
        add_walk_to_map(map, walk, len);
        return;
    }

    for (int neighbor = 0; neighbor < n_vertices; neighbor++) {
        if (adj[node][neighbor]) {
            int visited = 0;
            for (int i = 0; i <= depth; i++) {
                if (path[i] == neighbor) {
                    if (!(i == 0 && depth + 1 == len && neighbor == path[0])) {
                        visited = 1;
                        break;
                    }
                }
            }
            if (visited) continue;

            path[depth + 1] = neighbor;
            dfs(adj, n_vertices, len, neighbor, depth + 1, path, map);
        }
    }
}


WalkMapEntry* get_walks(int **adj, int n_vertices, int len) {
    WalkMapEntry *map = NULL;
    int *path = malloc((len + 1) * sizeof(int));

    for (int start = 0; start < n_vertices; start++) {
        path[0] = start;
        dfs(adj, n_vertices, len, start, 0, path, &map);
    }

    free(path);
    return map;
}

void print_walk_map(WalkMapEntry *map, int len) {
    WalkMapEntry *entry, *tmp;
    HASH_ITER(hh, map, entry, tmp) {
        printf("From %d to %d:\n", entry->key.start, entry->key.end);
        for (int i = 0; i < entry->count; i++) {
            for (int j = 0; j <= len; j++) {
                printf("%d ", entry->walk_list[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void free_walk_map(WalkMapEntry *map) {
    WalkMapEntry *entry, *tmp;
    HASH_ITER(hh, map, entry, tmp) {
        for (int i = 0; i < entry->count; i++) {
            free(entry->walk_list[i]);
        }
        free(entry->walk_list);
        HASH_DEL(map, entry);
        free(entry);
    }
}
