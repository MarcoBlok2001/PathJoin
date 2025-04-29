// walks.h
#ifndef WALKS_H
#define WALKS_H

#include <stdio.h>
#include <string.h>

int** get_walks(int **adj, int n, int k, int *count);

void print_walks(int **walks, int k, int count);

void free_walks(int **walks, int count);

#endif // WALKS_H