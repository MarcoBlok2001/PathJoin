/*
 * MIT License
 *
 * Copyright (c) 2025 Marco Blok
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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