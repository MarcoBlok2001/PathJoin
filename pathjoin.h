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

typedef struct {
    int *cycle;
    int len;
    UT_hash_handle hh;
} CycleSetEntry;

CycleSetEntry* path_join(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    int max_nodes,
    int *out_count,
    int verbose
);

CycleSetEntry* path_join_three(
    PathMapEntry *map1, int k1,
    PathMapEntry *map2, int k2,
    PathMapEntry *map3, int k3,
    int max_nodes,
    int *out_count,
    int verbose
);

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