#include "walkjoin.h"

typedef struct {
    int start, end;
} WalkKey;

typedef struct {
    WalkKey key;
    int **walk_list;
    int count;
    int capacity;
    UT_hash_handle hh;
} WalkMapEntry;

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

static int is_simple_cycle(int *walk, int k, int *seen, int max_nodes) {
    memset(seen, 0, max_nodes * sizeof(int));

    if (walk[0] != walk[k]) {
        return 0;
    }

    for (int i = 0; i < k; i++) {
        if (seen[walk[i]]) return 0;
        seen[walk[i]] = 1;
    }
    return 1;
}

int** walk_join_three(int **walks, int k, int n_walks, int max_nodes, int *out_count) {
    WalkMapEntry *map = NULL;
    int *seen = malloc(max_nodes * sizeof(int));
    int **result = malloc(n_walks * n_walks * n_walks * sizeof(int*));
    int count = 0;

    // Build map: (start, end) → walks
    for (int i = 0; i < n_walks; i++) {
        add_walk_to_map(&map, walks[i], k);
    }

    // For each w1: a→b
    WalkMapEntry *w1_entry, *tmp;
    HASH_ITER(hh, map, w1_entry, tmp) {
        for (int i = 0; i < w1_entry->count; i++) {
            int *w1 = w1_entry->walk_list[i];
            int a = w1[0];
            int b = w1[k];

            // Find all w2: b→c
            for (int j = 0; j < n_walks; j++) {
                int *w2 = walks[j];
                if (w2[0] != b) continue;

                int c = w2[k];

                // Look up w3: c→a
                WalkKey key3 = {c, a};
                WalkMapEntry *w3_entry = NULL;
                HASH_FIND(hh, map, &key3, sizeof(WalkKey), w3_entry);
                if (!w3_entry) continue;

                for (int m = 0; m < w3_entry->count; m++) {
                    int *w3 = w3_entry->walk_list[m];

                    // Avoid duplicate use of same walk
                    if (w3 == w1 || w3 == w2) continue;

                    int total_len = 3 * k;
                    int *joined = malloc((total_len + 1) * sizeof(int));

                    // Copy w1[0..k]
                    for (int x = 0; x <= k; x++) joined[x] = w1[x];
                    // Copy w2[1..k]
                    for (int x = 1; x <= k; x++) joined[k + x] = w2[x];
                    // Copy w3[1..k]
                    for (int x = 1; x <= k; x++) joined[2 * k + x] = w3[x];

                    if (is_simple_cycle(joined, total_len, seen, max_nodes)) {
                        result[count++] = joined;
                    } else {
                        free(joined);
                    }
                }
            }
        }
    }

    // Cleanup
    HASH_ITER(hh, map, w1_entry, tmp) {
        free(w1_entry->walk_list);
        HASH_DEL(map, w1_entry);
        free(w1_entry);
    }

    free(seen);
    *out_count = count;
    return result;
}


int** walk_join(int **walks, int k, int n_walks, int max_nodes, int *out_count) {
    WalkMapEntry *map = NULL;
    int *seen = malloc(max_nodes * sizeof(int));
    int **result = malloc(n_walks * n_walks * sizeof(int*)); // max possible

    // Build map
    for (int i = 0; i < n_walks; i++) {
        add_walk_to_map(&map, walks[i], k);
    }

    int count = 0;
    WalkMapEntry *entry, *reverse_entry, *tmp;
    HASH_ITER(hh, map, entry, tmp) {
        WalkKey rev_key = {entry->key.end, entry->key.start};
        HASH_FIND(hh, map, &rev_key, sizeof(WalkKey), reverse_entry);
        if (!reverse_entry) continue;

        for (int i = 0; i < entry->count; i++) {
            for (int j = 0; j < reverse_entry->count; j++) {
                int *w1 = entry->walk_list[i];
                int *w2 = reverse_entry->walk_list[j];

                int *joined = malloc((2 * k + 1) * sizeof(int));
                for (int x = 0; x <= k; x++) joined[x] = w1[x];
                for (int x = 1; x <= k; x++) joined[k + x] = w2[x];

                if (is_simple_cycle(joined, 2 * k, seen, max_nodes)) {
                    result[count++] = joined;
                } else {
                    free(joined);
                }
            }
        }
    }

    // Cleanup
    HASH_ITER(hh, map, entry, tmp) {
        free(entry->walk_list);
        HASH_DEL(map, entry);
        free(entry);
    }
    free(seen);

    *out_count = count;
    return result;
}
