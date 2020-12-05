#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include "readmtx.h" // readMtxFile
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray

void cooSequential(uint32_t *rowsCoo, uint32_t *colsCoo, uint32_t *c3, uint32_t nnz, uint32_t nc) {
    uint32_t **adj = (uint32_t **) malloc(nc * sizeof(uint32_t *));
    for (uint32_t i = 0; i < nc; i++)
        adj[i] = (uint32_t *) malloc(nc * sizeof(uint32_t));
    for (uint32_t i = 0; i < nc; i++)
        for (uint32_t j = 0; j < nc; j++)
            adj[i][j] = 0;
    for (uint32_t i = 0; i < nnz; i++) {
        adj[colsCoo[i]][rowsCoo[i]] = 1;
        adj[rowsCoo[i]][colsCoo[i]] = 1;
    }
    for (uint32_t i = 0; i < nc; i++)
        for (uint32_t j = i + 1; j < nc; j++)
            for (uint32_t k = j + 1; k < nc; k++)
                if (adj[i][j] == 1 && adj[j][k] == 1 && adj[k][i] == 1) {
                    c3[i]++;
                    c3[j]++;
                    c3[k]++;
                }
    for (uint32_t i = 0; i < nc; i++)
        free(adj[i]);
    free(adj);
}

void runAndPresentResult(uint32_t *rowsCsc, uint32_t *colsCsc, uint32_t nc, void (* runnable) (uint32_t *, uint32_t *, uint32_t *, uint32_t), char *name) {
    uint32_t *c3 = (uint32_t *)malloc(nc * sizeof(uint32_t));
    zeroOutArray(c3, nc);
    double time = measureTimeForRunnable(runnable, rowsCsc, colsCsc, c3, nc);
    uint32_t triangles = 0;
    for (uint32_t i = 0; i < nc; i++)
        triangles += c3[i];
    triangles /= 3;
    printf("-----------------------------------\n");
    printf("| Algorithm: %s\n", name);
    printf("| Time: %10.6lf\n", time);
    printf("| Triangles: %d\n", triangles);
    printf("-----------------------------------\n");
    free(c3);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [matrix-market-filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint32_t *rowsCoo, *colsCoo, nr, nc, nnz;

    readMtxFile(argv[1], &rowsCoo, &colsCoo, &nr, &nc, &nnz);

    uint32_t *c3 = (uint32_t *)malloc(nc * sizeof(uint32_t));
    zeroOutArray(c3, nc);
    cooSequential(rowsCoo, colsCoo, c3, nnz, nc);
    uint32_t triangles = 0;
    for (uint32_t i = 0; i < nc; i++)
        triangles += c3[i];
    triangles /= 3;
    printf("-----------------------------------\n");
    printf("| Triangles: %d\n", triangles);
    printf("-----------------------------------\n");
    free(c3);

    free(rowsCoo);
    free(colsCoo);
    return EXIT_SUCCESS;
}
