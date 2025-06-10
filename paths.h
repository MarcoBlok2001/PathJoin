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

// paths.h
#ifndef PATHS_H
#define PATHS_H

#include <stdio.h>
#include "uthash.h"

/**
 * @brief Key to identify paths by their start and end vertices.
 */
typedef struct {
    int start, end;
} PathKey;

/**
 * @brief Structure for storing multiple paths between two vertices.
 *
 * Uses uthash for quick lookup by PathKey.
 */
typedef struct {
    PathKey key;
    int **path_list;
    int count;
    int capacity;
    UT_hash_handle hh;
} PathMapEntry;

/**
 * @brief Finds all simple paths of length 'len' in the graph.
 *
 * @param adj         The adjacency matrix of the graph.
 * @param degrees     Array of vertex degrees (used to skip isolated vertices).
 * @param n_vertices  Number of vertices in the graph.
 * @param len         The exact length of paths to find (number of edges).
 * @param path_count  Pointer to integer where number of paths found can be stored
 * @return            A hash map of paths keyed by start and end vertices.
 */
PathMapEntry* get_paths(int **adj, int *degrees, int n_vertices, int len, int *path_count);

/**
 * @brief Prints all paths stored in the given PathMapEntry map.
 *
 * @param paths The map of paths to print.
 * @param len   The length of each path (number of edges).
 */
void print_path_map(PathMapEntry *paths, int len);

/**
 * @brief Frees all memory associated with the PathMapEntry map.
 *
 * @param paths The map of paths to free.
 */
void free_path_map(PathMapEntry *paths);

#endif // PATHS_H