// walks.h
#ifndef WALKS_H
#define WALKS_H

#include <stdio.h>
#include <string.h>
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

WalkMapEntry* get_walks(int **adj, int n_vertices, int len);

void print_walk_map(WalkMapEntry*walks, int len);

void free_walk_map(WalkMapEntry*walks);

#endif // WALKS_H