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

// pre_processing.c
#include "pre_processing.h"

int* twocores(int** adj, int *degrees, int num_vertices, int directed) {
    int changed = 1;

    while (changed == 1) {
        changed = 0;
        for (int i = 0; i < num_vertices; i++) {
            if (degrees[i] == 1) {
                for (int j = 0; j < num_vertices; j++) {
                    if (adj[i][j] == 1) {
                        changed = 1;
                        adj[i][j] = 0;
                        degrees[i]--;
                        degrees[j]--;

                        if (!directed) {
                            adj[j][i] = 0;  // Only for undirected graphs
                        }
                        break;
                    } else if (directed && adj[j][i] == 1) {
                        changed = 1;
                        adj[j][i] = 0;
                        degrees[j]--;
                        degrees[i]--;
                        break;
                    }
                }
            }
        }
    }

    // for (int i = 0; i < num_vertices; i++) {
    //     printf("%d ", degrees[i]);
    // }
    // printf("\n");

    return degrees;
}
