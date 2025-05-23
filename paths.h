// paths.h
#ifndef PATHS_H
#define PATHS_H

#include <stdio.h>
#include "uthash.h"

typedef struct {
    int start, end;
} PathKey;

typedef struct {
    PathKey key;
    int **path_list;
    int count;
    int capacity;
    UT_hash_handle hh;
} PathMapEntry;

PathMapEntry* get_paths(int **adj, int *degrees, int n_vertices, int len);

void print_path_map(PathMapEntry*paths, int len);

void free_path_map(PathMapEntry*paths);

#endif // PATHS_H