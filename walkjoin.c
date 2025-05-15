// walkjoin.c
#include "walkjoin.h"


void store_cycle(CycleSetEntry **set, int *cycle, int len) {
    CycleSetEntry *entry = malloc(sizeof(CycleSetEntry));
    entry->cycle = malloc(len * sizeof(int));
    memcpy(entry->cycle, cycle, len * sizeof(int));
    entry->len = len;
    HASH_ADD_KEYPTR(hh, *set, entry->cycle, len * sizeof(int), entry);
}

#include <stdlib.h>

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

int** walk_join(
    WalkMapEntry *map1, int k1,
    WalkMapEntry *map2, int k2,
    int max_nodes, int *out_count
) {
    int result_capacity = 128;
    int **result = malloc(result_capacity * sizeof(int*));
    int count = 0;

    int *seen = calloc(max_nodes, sizeof(int));  // zeroed on alloc
    CycleSetEntry *cycle_set = NULL;

    WalkMapEntry *entry1, *tmp1;
    HASH_ITER(hh, map1, entry1, tmp1) {
        // Reverse key to match end of walk1 with start of walk2
        WalkKey key = {entry1->key.end, entry1->key.start};
        WalkMapEntry *entry2;
        HASH_FIND(hh, map2, &key, sizeof(WalkKey), entry2);
        if (!entry2) continue;
        int total_len = k1 + k2 + 1;
        int *joined = malloc(total_len * sizeof(int));

        for (int i = 0; i < entry1->count; i++) {
            int *w1 = entry1->walk_list[i];

            for (int j = 0; j < entry2->count; j++) {
                int *w2 = entry2->walk_list[j];

                // Allocate joined buffer once

                // int *joined = malloc(total_len * sizeof(int));

                // Join: w1[0..k1] + w2[1..k2]
                memcpy(joined, w1, (k1 + 1) * sizeof(int));
                memcpy(joined + k1 + 1, w2 + 1, k2 * sizeof(int));

                // Validate simple cycle
                if (is_simple_cycle(joined, total_len - 1, seen, max_nodes)) {
                    int *canon = canonical_cycle(joined, total_len - 1);

                    if (!cycle_already_seen(cycle_set, canon, total_len - 1)) {
                        store_cycle(&cycle_set, canon, total_len - 1);

                        if (count == result_capacity) {
                            result_capacity *= 2;
                            result = realloc(result, result_capacity * sizeof(int*));
                        }

                        result[count++] = canon;
                        // free(joined);
                    } else {
                        free(canon);
                        // free(joined);
                    }
                } else {
                    // free(joined);
                }
            }
        }
        free(joined);
    }

    free(seen);

    // Free the cycle set
    CycleSetEntry *centry, *tmp;
    HASH_ITER(hh, cycle_set, centry, tmp) {
        HASH_DEL(cycle_set, centry);
        free(centry->cycle);
        free(centry);
    }

    *out_count = count;
    return result;
}

int** walk_join_three(
    WalkMapEntry *map1, int k1,
    WalkMapEntry *map2, int k2,
    WalkMapEntry *map3, int k3,
    int max_nodes,
    int *out_count
) {
    int result_capacity = 128;
    int **result = malloc(result_capacity * sizeof(int*));
    int count = 0;

    int *seen = calloc(max_nodes, sizeof(int));  // zeroed for is_simple_cycle
    CycleSetEntry *cycle_set = NULL;

    WalkMapEntry *entry1, *tmp1;
    HASH_ITER(hh, map1, entry1, tmp1) {
        int a = entry1->key.start;
        int b = entry1->key.end;

        // Find walks in map2 that start at b
        WalkMapEntry *entry2, *tmp2;
        HASH_ITER(hh, map2, entry2, tmp2) {
            if (entry2->key.start != b) continue;
            int c = entry2->key.end;

            // Look for walk c → a in map3
            WalkKey key3 = {c, a};
            WalkMapEntry *entry3 = NULL;
            HASH_FIND(hh, map3, &key3, sizeof(WalkKey), entry3);
            if (!entry3) continue;

            for (int i = 0; i < entry1->count; i++) {
                int *w1 = entry1->walk_list[i];

                for (int j = 0; j < entry2->count; j++) {
                    int *w2 = entry2->walk_list[j];
                    if (w2 == w1) continue;

                    for (int m = 0; m < entry3->count; m++) {
                        int *w3 = entry3->walk_list[m];
                        if (w3 == w1 || w3 == w2) continue;

                        int total_len = k1 + k2 + k3;
                        int *joined = malloc((total_len + 1) * sizeof(int));

                        // Copy w1[0..k1]
                        memcpy(joined, w1, (k1 + 1) * sizeof(int));
                        // Copy w2[1..k2]
                        memcpy(joined + k1 + 1, w2 + 1, k2 * sizeof(int));
                        // Copy w3[1..k3]
                        memcpy(joined + k1 + k2 + 1, w3 + 1, k3 * sizeof(int));

                        if (is_simple_cycle(joined, total_len, seen, max_nodes)) {
                            int *canon = canonical_cycle(joined, total_len);

                            if (!cycle_already_seen(cycle_set, canon, total_len)) {
                                store_cycle(&cycle_set, canon, total_len);
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
        }
    }

    free(seen);

    // Free the cycle set
    CycleSetEntry *centry, *tmp;
    HASH_ITER(hh, cycle_set, centry, tmp) {
        HASH_DEL(cycle_set, centry);
        free(centry->cycle);
        free(centry);
    }

    *out_count = count;
    return result;
}

int** walk_join_four(
    WalkMapEntry *map1, int k1,
    WalkMapEntry *map2, int k2,
    WalkMapEntry *map3, int k3,
    WalkMapEntry *map4, int k4,
    int max_nodes,
    int *out_count
) {
    int result_capacity = 128;
    int **result = malloc(result_capacity * sizeof(int*));
    int count = 0;

    int *seen = calloc(max_nodes, sizeof(int));  // for is_simple_cycle
    CycleSetEntry *cycle_set = NULL;

    WalkMapEntry *entry1, *tmp1;
    HASH_ITER(hh, map1, entry1, tmp1) {
        int a = entry1->key.start;
        int b = entry1->key.end;

        // Iterate over map2 where start == b
        WalkMapEntry *entry2, *tmp2;
        HASH_ITER(hh, map2, entry2, tmp2) {
            if (entry2->key.start != b) continue;
            int c = entry2->key.end;

            // Iterate over map3 where start == c
            WalkMapEntry *entry3, *tmp3;
            HASH_ITER(hh, map3, entry3, tmp3) {
                if (entry3->key.start != c) continue;
                int d = entry3->key.end;

                // Look up map4 where d → a
                WalkKey key4 = {d, a};
                WalkMapEntry *entry4 = NULL;
                HASH_FIND(hh, map4, &key4, sizeof(WalkKey), entry4);
                if (!entry4) continue;

                for (int i = 0; i < entry1->count; i++) {
                    int *w1 = entry1->walk_list[i];

                    for (int j = 0; j < entry2->count; j++) {
                        int *w2 = entry2->walk_list[j];
                        if (w2 == w1) continue;

                        for (int m = 0; m < entry3->count; m++) {
                            int *w3 = entry3->walk_list[m];
                            if (w3 == w1 || w3 == w2) continue;

                            for (int n = 0; n < entry4->count; n++) {
                                int *w4 = entry4->walk_list[n];
                                if (w4 == w1 || w4 == w2 || w4 == w3) continue;

                                int total_len = k1 + k2 + k3 + k4;
                                int *joined = malloc((total_len + 1) * sizeof(int));

                                // w1[0..k1]
                                memcpy(joined, w1, (k1 + 1) * sizeof(int));
                                // w2[1..k2]
                                memcpy(joined + k1 + 1, w2 + 1, k2 * sizeof(int));
                                // w3[1..k3]
                                memcpy(joined + k1 + k2 + 1, w3 + 1, k3 * sizeof(int));
                                // w4[1..k4]
                                memcpy(joined + k1 + k2 + k3 + 1, w4 + 1, k4 * sizeof(int));

                                if (is_simple_cycle(joined, total_len, seen, max_nodes)) {
                                    int *canon = canonical_cycle(joined, total_len);
                                    if (!cycle_already_seen(cycle_set, canon, total_len)) {
                                        store_cycle(&cycle_set, canon, total_len);
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
                }
            }
        }
    }

    free(seen);

    // Free cycle set
    CycleSetEntry *centry, *tmp;
    HASH_ITER(hh, cycle_set, centry, tmp) {
        HASH_DEL(cycle_set, centry);
        free(centry->cycle);
        free(centry);
    }

    *out_count = count;
    return result;
}


