// pathjoin.h
#ifndef PATHJOIN_H
#define PATHJOIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uthash.h"
#include "paths.h"

typedef struct {
    int *cycle;
    int len;
    UT_hash_handle hh;
} CycleSetEntry;

int** path_join(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    int max_nodes,
    int *out_count);

int** path_join_three(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    PathMapEntry *map3, int k3,
    int max_nodes,
    int *out_count
);

int** path_join_four(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    PathMapEntry *map3, int k3,
    PathMapEntry *map4, int k4,
    int max_nodes,
    int *out_count
);

#endif // PATHWRAP_H