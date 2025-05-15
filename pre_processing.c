// pre_processing.c
#include "pre_processing.h"

int* twocores(int** adj, int *degrees, int num_vertices) {
    int changed = 1;

    while (changed == 1) {
        changed = 0;
        for (int i = 0; i < num_vertices; i++) {
            if (degrees[i] == 1) {
                for (int j = 0; j < num_vertices; j++) {
                    if (adj[i][j] == 1) {
                        changed = 1;
                        adj[i][j] = 0;
                        adj[j][i] = 0;
                        degrees[i] = 0;
                        degrees[j] -= 1;
                        break;
                    }
                }
            }
        }
    }

    for (int i = 0; i < num_vertices; i++) {
        printf("%d ", degrees[i]);
    }
    printf("\n");

    return degrees;
}