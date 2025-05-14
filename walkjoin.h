// walkjoin.h
#ifndef WALKJOIN_H
#define WALKJOIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uthash.h"

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

typedef struct {
    int *cycle;
    int len;
    UT_hash_handle hh;
} CycleSetEntry;

int** walk_join(WalkMapEntry *map, int walk_len, int max_nodes, int *out_count);

int** walk_join_three(int **walks, int k, int n_walks, int max_nodes, int *out_count);

int** walk_join_mixed(WalkMapEntry *map1, int k1, WalkMapEntry *map2, int k2, int max_nodes, int *out_count);

int** walk_join_four(int **walks, int k, int n_walks, int max_nodes, int *out_count);

#endif // WALKWRAP_H