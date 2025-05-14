// walks.h
#ifndef WALKS_H
#define WALKS_H

#include <stdio.h>
#include <string.h>
#include "walkjoin.h"

WalkMapEntry* get_walks(int **adj, int n_vertices, int len);

void print_walk_map(WalkMapEntry*walks, int len);

void free_walk_map(WalkMapEntry*walks);

#endif // WALKS_H