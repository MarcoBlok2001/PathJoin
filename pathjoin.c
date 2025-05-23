// pathjoin.c
#include "pathjoin.h"


void store_cycle(CycleSetEntry **set, int *cycle, int len) {
    CycleSetEntry *entry = malloc(sizeof(CycleSetEntry));
    entry->cycle = malloc(len * sizeof(int));
    memcpy(entry->cycle, cycle, len * sizeof(int));
    entry->len = len;
    HASH_ADD_KEYPTR(hh, *set, entry->cycle, len * sizeof(int), entry);
}

// Rotate the cycle so that start_index becomes the first element
void rotate_cycle(int *cycle, int len, int start_index, int *result) {
    for (int i = 0; i <= len; i++) {
        result[i] = cycle[(start_index + i) % len];
    }
}

// Reverse an array in place
void reverse_in_place(int *cycle, int len) {
    for (int i = 0; i <= len / 2; i++) {
        int tmp = cycle[i];
        cycle[i] = cycle[len - i];
        cycle[len - i] = tmp;
    }
}

// Canonicalize the cycle
int* canonical_cycle(int *cycle, int len) {
    int min_index = 0;
    int min_val = cycle[0];

    // Find the index of the minimum value
    for (int i = 1; i < len; i++) {
        if (cycle[i] < min_val) {
            min_val = cycle[i];
            min_index = i;
        }
    }

    int left_index = (min_index - 1 + len) % len;
    int right_index = (min_index + 1) % len;

    int diff_l = cycle[min_index] - cycle[left_index];
    if (diff_l < 0) diff_l = -diff_l;

    int diff_r = cycle[right_index] - cycle[min_index];
    if (diff_r < 0) diff_r = -diff_r;

    int *result = (int*)malloc((len + 1) * sizeof(int));

    // Rotate first
    rotate_cycle(cycle, len, min_index, result);

    // Reverse in-place if left diff is smaller
    if (diff_l < diff_r) {
        reverse_in_place(result, len);
    }

    return result;
}



int cycle_already_seen(CycleSetEntry *set, int *cycle, int len) {
    CycleSetEntry *entry;
    HASH_FIND(hh, set, cycle, len * sizeof(int), entry);
    return entry != NULL;
}

static int is_simple_cycle(int *path, int k, int *seen, int max_nodes) {
    memset(seen, 0, max_nodes * sizeof(int));

    if (path[0] != path[k]) {
        return 0;
    }

    for (int i = 0; i < k; i++) {
        if (seen[path[i]]) return 0;
        seen[path[i]] = 1;
    }
    return 1;
}

CycleSetEntry* path_join(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    int max_nodes,
    int *out_count,
    int verbose
) {
    int count = 0;

    int *seen = calloc(max_nodes, sizeof(int));  // zeroed on alloc
    CycleSetEntry *cycle_set = NULL;

    int total_len = k1 + k2 + 1;
    int *joined = malloc(total_len * sizeof(int));

    PathMapEntry *entry1, *tmp1;
    HASH_ITER(hh, map1, entry1, tmp1) {
        // Reverse key to match end of path1 with start of path2
        PathKey key = {entry1->key.end, entry1->key.start};
        PathMapEntry *entry2;
        HASH_FIND(hh, map2, &key, sizeof(PathKey), entry2);
        if (!entry2) continue;

        for (int i = 0; i < entry1->count; i++) {
            int *w1 = entry1->path_list[i];

            for (int j = 0; j < entry2->count; j++) {
                int *w2 = entry2->path_list[j];

                // Join: w1[0..k1] + w2[1..k2]
                memcpy(joined, w1, (k1 + 1) * sizeof(int));
                memcpy(joined + k1 + 1, w2 + 1, k2 * sizeof(int));
                // printf("joining");

                // Validate simple cycle
                if (is_simple_cycle(joined, total_len - 1, seen, max_nodes)) {
                    int *canon = canonical_cycle(joined, total_len - 1);

                    if (!cycle_already_seen(cycle_set, canon, total_len)) {
                        if (verbose) {
                            if (count % 1000 == 0) {
                                printf("\rEnumerating cycles: %d", count);
                                fflush(stdout);
                            }
                        }
                        store_cycle(&cycle_set, canon, total_len);
                        count++;
                    } else {
                        free(canon);
                    }
                }
            }
        }
    }

    free(seen);
    free(joined);

    *out_count = count;
    return cycle_set;
}

CycleSetEntry* path_join_three(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    PathMapEntry *map3, int k3,
    int max_nodes,
    int *out_count,
    int verbose
) {
    int count = 0;

    int *seen = calloc(max_nodes, sizeof(int));  // zeroed for is_simple_cycle
    CycleSetEntry *cycle_set = NULL;

    int total_len = k1 + k2 + k3 + 1;
    int *joined = malloc((total_len) * sizeof(int));

    PathMapEntry *entry1, *tmp1;
    HASH_ITER(hh, map1, entry1, tmp1) {
        int a = entry1->key.start;
        int b = entry1->key.end;

        // Find paths in map2 that start at b
        PathMapEntry *entry2, *tmp2;
        HASH_ITER(hh, map2, entry2, tmp2) {
            if (entry2->key.start != b) continue;
            int c = entry2->key.end;

            // Look for path c → a in map3
            PathKey key3 = {c, a};
            PathMapEntry *entry3 = NULL;
            HASH_FIND(hh, map3, &key3, sizeof(PathKey), entry3);
            if (!entry3) continue;

            for (int i = 0; i < entry1->count; i++) {
                int *w1 = entry1->path_list[i];

                for (int j = 0; j < entry2->count; j++) {
                    int *w2 = entry2->path_list[j];
                    if (w2 == w1) continue;

                    for (int m = 0; m < entry3->count; m++) {
                        int *w3 = entry3->path_list[m];
                        if (w3 == w1 || w3 == w2) continue;


                        // Copy w1[0..k1]
                        memcpy(joined, w1, (k1 + 1) * sizeof(int));
                        // Copy w2[1..k2]
                        memcpy(joined + k1 + 1, w2 + 1, k2 * sizeof(int));
                        // Copy w3[1..k3]
                        memcpy(joined + k1 + k2 + 1, w3 + 1, k3 * sizeof(int));

                        if (is_simple_cycle(joined, total_len - 1, seen, max_nodes)) {
                            int *canon = canonical_cycle(joined, total_len - 1);

                            if (!cycle_already_seen(cycle_set, canon, total_len)) {
                                if (verbose) {
                                    if (count % 1000 == 0) {
                                        printf("\rEnumerating cycles: %d", count);
                                        fflush(stdout);
                                    }
                                }
                                store_cycle(&cycle_set, canon, total_len);
                                count++;
                            } else {
                                free(canon);
                            }
                        }
                    }
                }
            }
        }
    }

    free(seen);
    free(joined);

    *out_count = count;
    return cycle_set;
}

CycleSetEntry* path_join_four(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    PathMapEntry *map3, int k3,
    PathMapEntry *map4, int k4,
    int max_nodes,
    int *out_count,
    int verbose
) {
    int count = 0;

    int *seen = calloc(max_nodes, sizeof(int));  // for is_simple_cycle
    CycleSetEntry *cycle_set = NULL;

    int total_len = k1 + k2 + k3 + k4 + 1;
    int *joined = malloc((total_len) * sizeof(int));

    PathMapEntry *entry1, *tmp1;
    HASH_ITER(hh, map1, entry1, tmp1) {
        int a = entry1->key.start;
        int b = entry1->key.end;

        // Iterate over map2 where start == b
        PathMapEntry *entry2, *tmp2;
        HASH_ITER(hh, map2, entry2, tmp2) {
            if (entry2->key.start != b) continue;
            int c = entry2->key.end;

            // Iterate over map3 where start == c
            PathMapEntry *entry3, *tmp3;
            HASH_ITER(hh, map3, entry3, tmp3) {
                if (entry3->key.start != c) continue;
                int d = entry3->key.end;

                // Look up map4 where d → a
                PathKey key4 = {d, a};
                PathMapEntry *entry4 = NULL;
                HASH_FIND(hh, map4, &key4, sizeof(PathKey), entry4);
                if (!entry4) continue;

                for (int i = 0; i < entry1->count; i++) {
                    int *w1 = entry1->path_list[i];

                    for (int j = 0; j < entry2->count; j++) {
                        int *w2 = entry2->path_list[j];
                        if (w2 == w1) continue;

                        for (int m = 0; m < entry3->count; m++) {
                            int *w3 = entry3->path_list[m];
                            if (w3 == w1 || w3 == w2) continue;

                            for (int n = 0; n < entry4->count; n++) {
                                int *w4 = entry4->path_list[n];
                                if (w4 == w1 || w4 == w2 || w4 == w3) continue;

                                // w1[0..k1]
                                memcpy(joined, w1, (k1 + 1) * sizeof(int));
                                // w2[1..k2]
                                memcpy(joined + k1 + 1, w2 + 1, k2 * sizeof(int));
                                // w3[1..k3]
                                memcpy(joined + k1 + k2 + 1, w3 + 1, k3 * sizeof(int));
                                // w4[1..k4]
                                memcpy(joined + k1 + k2 + k3 + 1, w4 + 1, k4 * sizeof(int));

                                if (is_simple_cycle(joined, total_len - 1, seen, max_nodes)) {
                                    int *canon = canonical_cycle(joined, total_len - 1);
                                    if (!cycle_already_seen(cycle_set, canon, total_len)) {
                                        if (verbose) {
                                            if (count % 1000 == 0) {
                                                printf("\rEnumerating cycles: %d", count);
                                                fflush(stdout);
                                            }
                                        }
                                        store_cycle(&cycle_set, canon, total_len);
                                        count++;
                                    } else {
                                        free(canon);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    free(seen);
    free(joined);

    *out_count = count;
    return cycle_set;
}


