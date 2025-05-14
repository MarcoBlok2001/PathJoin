// walkjoin.h
#ifndef WALKJOIN_H
#define WALKJOIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uthash.h"
#include "walks.h"

typedef struct {
    int *cycle;
    int len;
    UT_hash_handle hh;
} CycleSetEntry;

int** walk_join(
    WalkMapEntry *map1, int k1,
    WalkMapEntry *map2, int k2,
    int max_nodes,
    int *out_count);

int** walk_join_three(
    WalkMapEntry *map1, int k1,
    WalkMapEntry *map2, int k2,
    WalkMapEntry *map3, int k3,
    int max_nodes,
    int *out_count
);

int** walk_join_four(
    WalkMapEntry *map1, int k1,
    WalkMapEntry *map2, int k2,
    WalkMapEntry *map3, int k3,
    WalkMapEntry *map4, int k4,
    int max_nodes,
    int *out_count
);

#endif // WALKWRAP_H