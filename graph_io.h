// graph_io.h
#ifndef GRAPH_IO_H
#define GRAPH_IO_H

#include <stdio.h>

// Reads a Pajek-style .net file and returns an adjacency matrix.
// Fills in the number of vertices via pointer.
// The returned matrix should be freed by the caller.
int** parse(FILE *file, int *n, int directed);

int * count_degrees(int **adj, int n, int directed);

// Prints an N x N adjacency matrix.
void print_adjacency_matrix(int **adj, int n);

// Frees an N x N adjacency matrix.
void free_adjacency_matrix(int **adj, int *degrees, int n);

#endif // GRAPH_IO_H