// walkjoin.c
#include "walkjoin.h"

int compare_cycles(const int *a, const int *b, int len) {
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
    }
    return 0;
}

void store_cycle(CycleSetEntry **set, int *cycle, int len) {
    CycleSetEntry *entry = malloc(sizeof(CycleSetEntry));
    entry->cycle = malloc(len * sizeof(int));
    memcpy(entry->cycle, cycle, len * sizeof(int));
    entry->len = len;
    HASH_ADD_KEYPTR(hh, *set, entry->cycle, len * sizeof(int), entry);
}

// Canonicalize a cycle: lex smallest among all rotations and reversed rotations
int* canonical_cycle(int *cycle, int len) {
    int min_val = cycle[0];
    for (int i = 1; i < len; i++) {
        if (cycle[i] < min_val)
            min_val = cycle[i];
    }

    int *min_cycle = malloc((len + 1) * sizeof(int));
    int *temp = malloc(len * sizeof(int));

    // Initialize with original as baseline
    for (int i = 0; i < len; i++)
        min_cycle[i] = cycle[i];
    min_cycle[len] = min_cycle[0];

    // Check all forward rotations starting at min_val
    for (int i = 0; i < len; i++) {
        if (cycle[i] != min_val) continue;

        for (int j = 0; j < len; j++)
            temp[j] = cycle[(i + j) % len];

        if (compare_cycles(temp, min_cycle, len) < 0) {
            memcpy(min_cycle, temp, len * sizeof(int));
            min_cycle[len] = min_cycle[0];
        }
    }

    // Reverse the cycle
    for (int i = 0; i < len; i++)
        temp[i] = cycle[len - 1 - i];

    // Check reversed rotations starting at min_val
    for (int i = 0; i < len; i++) {
        if (temp[i] != min_val) continue;

        int rotated[len];
        for (int j = 0; j < len; j++)
            rotated[j] = temp[(i + j) % len];

        if (compare_cycles(rotated, min_cycle, len) < 0) {
            memcpy(min_cycle, rotated, len * sizeof(int));
            min_cycle[len] = min_cycle[0];
        }
    }

    free(temp);
    return min_cycle;
}

int cycle_already_seen(CycleSetEntry *set, int *cycle, int len) {
    CycleSetEntry *entry;
    HASH_FIND(hh, set, cycle, len * sizeof(int), entry);
    return entry != NULL;
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

int** walk_join_mixed(
    WalkMapEntry *map1, int k1,
    WalkMapEntry *map2, int k2,
    int max_nodes, int *out_count
) {
    int result_capacity = 128;
    int **result = malloc(result_capacity * sizeof(int*));
    int count = 0;

    int *seen = malloc(max_nodes * sizeof(int));
    CycleSetEntry *cycle_set = NULL;

    WalkMapEntry *entry1, *tmp1;

    HASH_ITER(hh, map1, entry1, tmp1) {
        WalkKey key = {entry1->key.end, entry1->key.start};  // reverse to match end of w1 to start of w2
        WalkMapEntry *entry2;
        HASH_FIND(hh, map2, &key, sizeof(WalkKey), entry2);
        if (!entry2) continue;

        for (int i = 0; i < entry1->count; i++) {
            for (int j = 0; j < entry2->count; j++) {
                int *w1 = entry1->walk_list[i];
                int *w2 = entry2->walk_list[j];

                int len = k1 + k2 + 1;
                int *joined = malloc(len * sizeof(int));

                for (int x = 0; x <= k1; x++) joined[x] = w1[x];
                for (int x = 1; x <= k2; x++) joined[k1 + x] = w2[x];

                if (is_simple_cycle(joined, len - 1, seen, max_nodes)) {
                    int *canon = canonical_cycle(joined, len - 1);
                    if (!cycle_already_seen(cycle_set, canon, len - 1)) {
                        store_cycle(&cycle_set, canon, len - 1);
                        free(canon);

                        if (count >= result_capacity) {
                            result_capacity *= 2;
                            result = realloc(result, result_capacity * sizeof(int*));
                        }

                        result[count++] = joined;
                    } else {
                        free(canon);
                        free(joined);
                    }
                } else {
                    free(joined);
                }
            }
        }
    }

    free(seen);

    CycleSetEntry *centry, *tmp_entry;
    HASH_ITER(hh, cycle_set, centry, tmp_entry) {
        HASH_DEL(cycle_set, centry);
        free(centry->cycle);
        free(centry);
    }

    *out_count = count;
    return result;
}



int** walk_join(WalkMapEntry *map, int k, int max_nodes, int *out_count) {
    int result_capacity = 128; // start with a reasonable initial capacity
    int **result = malloc(result_capacity * sizeof(int*));
    int count = 0;

    int *seen = malloc(max_nodes * sizeof(int));

    CycleSetEntry *cycle_set = NULL;

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
                    int *canon = canonical_cycle(joined, 2 * k);
                    if (!cycle_already_seen(cycle_set, canon, 2 * k)) {
                        store_cycle(&cycle_set, canon, 2 * k);
                        free(canon);

                        if (count == result_capacity) {
                            result_capacity *= 2;
                            result = realloc(result, result_capacity * sizeof(int*));
                        }

                        result[count++] = joined;
                    } else {
                        free(canon);
                        free(joined);
                    }
                } else {
                    free(joined);
                }
            }
        }
    }

    CycleSetEntry *centry, *tmp_entry;
    HASH_ITER(hh, cycle_set, centry, tmp_entry) {
        HASH_DEL(cycle_set, centry);
        free(centry->cycle);
        free(centry);
    }

    *out_count = count;
    return result;
}


// int** walk_join_four(int **walks, int k, int n_walks, int max_nodes, int *out_count) {
//     WalkMapEntry *map = NULL;
//     int *seen = malloc(max_nodes * sizeof(int));
//     int **result = malloc(n_walks * n_walks * n_walks * n_walks * sizeof(int*));
//     int count = 0;

//     // Build map (start, end) -> walk list
//     for (int i = 0; i < n_walks; i++) {
//         add_walk_to_map(&map, walks[i], k);
//     }

//     WalkMapEntry *w1_entry, *tmp;
//     HASH_ITER(hh, map, w1_entry, tmp) {
//         for (int i = 0; i < w1_entry->count; i++) {
//             int *w1 = w1_entry->walk_list[i];
//             int a = w1[0];
//             int b = w1[k];

//             // Find all w2: b -> c
//             for (int j = 0; j < n_walks; j++) {
//                 int *w2 = walks[j];
//                 if (w2[0] != b) continue;
//                 int c = w2[k];

//                 // Find all w3: c -> d
//                 for (int m = 0; m < n_walks; m++) {
//                     int *w3 = walks[m];
//                     if (w3[0] != c) continue;
//                     int d = w3[k];

//                     // Find w4: d -> a
//                     WalkKey key4 = {d, a};
//                     WalkMapEntry *w4_entry = NULL;
//                     HASH_FIND(hh, map, &key4, sizeof(WalkKey), w4_entry);
//                     if (!w4_entry) continue;

//                     for (int n = 0; n < w4_entry->count; n++) {
//                         int *w4 = w4_entry->walk_list[n];

//                         // Avoid duplicate walks
//                         if (w4 == w1 || w4 == w2 || w4 == w3) continue;

//                         int total_len = 4 * k;
//                         int *joined = malloc((total_len + 1) * sizeof(int));

//                         // w1[0..k]
//                         for (int x = 0; x <= k; x++) joined[x] = w1[x];
//                         // w2[1..k]
//                         for (int x = 1; x <= k; x++) joined[k + x] = w2[x];
//                         // w3[1..k]
//                         for (int x = 1; x <= k; x++) joined[2 * k + x] = w3[x];
//                         // w4[1..k]
//                         for (int x = 1; x <= k; x++) joined[3 * k + x] = w4[x];

//                         if (is_simple_cycle(joined, total_len, seen, max_nodes)) {
//                             if (count % 100000 == 0) printf("%d\n", count);
//                             result[count++] = joined;
//                         } else {
//                             free(joined);
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     // Cleanup
//     HASH_ITER(hh, map, w1_entry, tmp) {
//         free(w1_entry->walk_list);
//         HASH_DEL(map, w1_entry);
//         free(w1_entry);
//     }

//     free(seen);
//     *out_count = count;
//     return result;
// }


// int** walk_join_three(int **walks, int k, int n_walks, int max_nodes, int *out_count) {
//     WalkMapEntry *map = NULL;
//     int *seen = malloc(max_nodes * sizeof(int));
//     int **result = malloc(n_walks * n_walks * n_walks * sizeof(int*));
//     int count = 0;

//     // Build map: (start, end) → walks
//     for (int i = 0; i < n_walks; i++) {
//         add_walk_to_map(&map, walks[i], k);
//     }

//     // For each w1: a→b
//     WalkMapEntry *w1_entry, *tmp;
//     HASH_ITER(hh, map, w1_entry, tmp) {
//         for (int i = 0; i < w1_entry->count; i++) {
//             int *w1 = w1_entry->walk_list[i];
//             int a = w1[0];
//             int b = w1[k];

//             // Find all w2: b→c
//             for (int j = 0; j < n_walks; j++) {
//                 int *w2 = walks[j];
//                 if (w2[0] != b) continue;

//                 int c = w2[k];

//                 // Look up w3: c→a
//                 WalkKey key3 = {c, a};
//                 WalkMapEntry *w3_entry = NULL;
//                 HASH_FIND(hh, map, &key3, sizeof(WalkKey), w3_entry);
//                 if (!w3_entry) continue;

//                 for (int m = 0; m < w3_entry->count; m++) {
//                     int *w3 = w3_entry->walk_list[m];

//                     // Avoid duplicate use of same walk
//                     if (w3 == w1 || w3 == w2) continue;

//                     int total_len = 3 * k;
//                     int *joined = malloc((total_len + 1) * sizeof(int));

//                     // Copy w1[0..k]
//                     for (int x = 0; x <= k; x++) joined[x] = w1[x];
//                     // Copy w2[1..k]
//                     for (int x = 1; x <= k; x++) joined[k + x] = w2[x];
//                     // Copy w3[1..k]
//                     for (int x = 1; x <= k; x++) joined[2 * k + x] = w3[x];

//                     if (is_simple_cycle(joined, total_len, seen, max_nodes)) {
//                         if (count % 100 == 0) printf("%d\n", count);
//                         result[count++] = joined;
//                     } else {
//                         free(joined);
//                     }
//                 }
//             }
//         }
//     }

//     // Cleanup
//     HASH_ITER(hh, map, w1_entry, tmp) {
//         free(w1_entry->walk_list);
//         HASH_DEL(map, w1_entry);
//         free(w1_entry);
//     }

//     free(seen);
//     *out_count = count;
//     return result;
// }