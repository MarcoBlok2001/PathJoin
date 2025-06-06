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

// pathjoin.h
#ifndef PATHJOIN_H
#define PATHJOIN_H

#include <stdio.h>
#include <stdlib.h>

#include "uthash.h"
#include "paths.h"

/**
 * @brief Structure representing a cycle entry in a hash set.
 */
typedef struct {
    int *cycle;
    int len;
    UT_hash_handle hh;
} CycleSetEntry;

/**
 * @brief Joins two path maps to enumerate simple cycles formed by concatenating paths.
 *
 * @param map1 Pointer to the first path map
 * @param k1 Length of paths in the first map (i.e., number of edges)
 * @param map2 Pointer to the second path map
 * @param k2 Length of paths in the second map
 * @param max_nodes Maximum number of nodes in the graph (used for visited array)
 * @param out_count Pointer to an integer where the number of cycles found will be stored
 * @param verbose If non-zero, enables verbose output during enumeration
 * @return Pointer to a hash set of unique canonicalized cycles found
 */
CycleSetEntry* path_join(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    int max_nodes,
    int *out_count,
    int verbose
);

/**
 * @brief Joins three path maps to enumerate simple cycles formed by concatenating paths.
 *
 * @param map1 Pointer to the first path map
 * @param k1 Length of paths in the first map
 * @param map2 Pointer to the second path map
 * @param k2 Length of paths in the second map
 * @param map3 Pointer to the third path map
 * @param k3 Length of paths in the third map
 * @param max_nodes Maximum number of nodes in the graph (used for visited array)
 * @param out_count Pointer to an integer where the number of cycles found will be stored
 * @param verbose If non-zero, enables verbose output during enumeration
 * @return Pointer to a hash set of unique canonicalized cycles found
 */
CycleSetEntry* path_join_three(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    PathMapEntry *map3, int k3,
    int max_nodes,
    int *out_count,
    int verbose
);

/**
 * @brief Joins four path maps to enumerate simple cycles formed by concatenating paths.
 *
 * @param map1 Pointer to the first path map
 * @param k1 Length of paths in the first map
 * @param map2 Pointer to the second path map
 * @param k2 Length of paths in the second map
 * @param map3 Pointer to the third path map
 * @param k3 Length of paths in the third map
 * @param map4 Pointer to the fourth path map
 * @param k4 Length of paths in the fourth map
 * @param max_nodes Maximum number of nodes in the graph (used for visited array)
 * @param out_count Pointer to an integer where the number of cycles found will be stored
 * @param verbose If non-zero, enables verbose output during enumeration
 * @return Pointer to a hash set of unique canonicalized cycles found
 */
CycleSetEntry* path_join_four(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    PathMapEntry *map3, int k3,
    PathMapEntry *map4, int k4,
    int max_nodes,
    int *out_count,
    int verbose
);

#endif // PATHJOIN_H