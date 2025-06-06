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

// pre_processing.h
#ifndef PRE_PROCESSING_H
#define PRE_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Prunes all vertices of degree 1 from the graph iteratively to compute the 2-core.
 *
 * The function modifies the adjacency matrix and degree array in-place.
 * In a 2-core, all vertices have degree at least 2 (or 0 if removed).
 *
 * @param adj           The adjacency matrix representing the graph.
 * @param degrees       Array of vertex degrees, updated in-place.
 * @param num_vertices  Total number of vertices in the graph.
 * @param directed      Non-zero if the graph is directed; 0 if undirected.
 * @return              Pointer to the updated degrees array (same as input).
 */
int* twocores(int** adj, int *degrees, int num_vertices, int directed);

#endif // PRE_PROCESSING_H