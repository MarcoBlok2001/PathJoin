// walkwrap.c
#include <stdlib.h>
#include "walkwrap.h"


static int is_simple_cycle(int *walk, int k, int *seen, int max_nodes) {
    memset(seen, 0, max_nodes * sizeof(int));

    if (walk[0] != walk[k]) {
        return 0;
    }
    for (int i = 0; i < k; i++) {
        if (seen[walk[i]]) return 0;
        seen[walk[i]] = 1;
    }
    return 1;
}


int** walk_wrap(int **walks, int k, int n, int max_nodes, int *out_count) {
    int **result = malloc(n * sizeof(int*));  // At most n cycles
    int count = 0;

    int *seen = calloc(max_nodes, sizeof(int));
    if (!seen) return NULL;

    for (int i = 0; i < n; i++) {
        if (is_simple_cycle(walks[i], k, seen, max_nodes)) {
            result[count] = malloc((k+1) * sizeof(int));
            memcpy(result[count], walks[i], (k+1) * sizeof(int));
            count++;
        }
    }

    free(seen);

    if (out_count) {
        *out_count = count;
    }
    return result;
}
