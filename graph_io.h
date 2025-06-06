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

/**
 * @brief Parses a graph file in Pajek .net format and builds an adjacency matrix.
 *
 * @param file      Pointer to an open file containing the graph.
 * @param n         Output parameter that receives the number of vertices (+1 if 1-based indexing).
 * @param directed  Non-zero if the graph is directed; 0 if undirected.
 * @return          Allocated n x n adjacency matrix.
 *                  Returns NULL on error. Caller is responsible for freeing the matrix.
 */
int** parse(FILE *file, int *n, int directed);

/**
 * @brief Computes degrees for each vertex in the graph.
 *
 * @param adj       Adjacency matrix of the graph.
 * @param n         Number of vertices.
 * @param directed  Non-zero for directed graphs (in-degree + out-degree), 0 for undirected.
 * @return          Allocated array of vertex degrees.
 *                  Caller must free the returned array.
 */
int * count_degrees(int **adj, int n, int directed);

/**
 * @brief Prints the adjacency matrix to stdout.
 *
 * @param adj       Adjacency matrix to print.
 * @param n         Number of vertices (matrix is n x n).
 */
void print_adjacency_matrix(int **adj, int n);

/**
 * @brief Frees the memory allocated for the adjacency matrix and degree array.
 *
 * @param adj       Adjacency matrix to free.
 * @param degrees   Array of degrees to free.
 * @param n         Number of vertices.
 */
void free_adjacency_matrix(int **adj, int *degrees, int n);

#endif // GRAPH_IO_H