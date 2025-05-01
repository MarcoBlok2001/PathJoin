// walkjoin.h
#ifndef WALKJOIN_H
#define WALKJOIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uthash.h"

int** walk_join(int **walks, int walk_len, int n, int max_nodes, int *out_count);

int** walk_join_three(int **walks, int k, int n_walks, int max_nodes, int *out_count);

int** walk_join_mixed(int **walks1, int k1, int n1,
    int **walks2, int k2, int n2,
    int max_nodes, int *out_count);

int** walk_join_four(int **walks, int k, int n_walks, int max_nodes, int *out_count);

#endif // WALKWRAP_H