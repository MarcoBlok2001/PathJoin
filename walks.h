// walks.h
#ifndef WALKS_H
#define WALKS_H

#include <stdio.h>
#include <string.h>

int** get_walks(int **adj, int n_vertices, int len, int *count);

void print_walks(int **walks, int len, int count);

void free_walks(int **walks, int count);

#endif // WALKS_H