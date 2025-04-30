// walkjoin.h
#ifndef WALKJOIN_H
#define WALKJOIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uthash.h"

int** walk_join(int **walks, int walk_len, int n, int max_nodes, int *out_count);

#endif // WALKWRAP_H