#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include "readmtx.h" // readMtxFile
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray

void cooSequential(int *rowsCoo, int *colsCoo, int *c3, int nnz, int nc) {
    int **adj = (int **) malloc(nc * sizeof(int *));
    for (int i = 0; i < nc; i++)
        adj[i] = (int *) malloc(nc * sizeof(int));
    for (int i = 0; i < nc; i++)
        for (int j = 0; j < nc; j++)
            adj[i][j] = 0;
    for (int i = 0; i < nnz; i++) {
        adj[colsCoo[i]][rowsCoo[i]] = 1;
        adj[rowsCoo[i]][colsCoo[i]] = 1;
    }
    for (int i = 0; i < nc; i++)
        for (int j = i + 1; j < nc; j++)
            for (int k = j + 1; k < nc; k++)
                if (adj[i][j] == 1 && adj[j][k] == 1 && adj[k][i] == 1) {
                    c3[i]++;
                    c3[j]++;
                    c3[k]++;
                }
    for (int i = 0; i < nc; i++)
        free(adj[i]);
    free(adj);
}

void runAndPresentResult(int *rowsCsc, int *colsCsc, int nc, void (* runnable) (int *, int *, int *, int), char *name) {
    int *c3 = (int *)malloc(nc * sizeof(int));
    zeroOutArray(c3, nc);
    double time = measureTimeForRunnable(runnable, rowsCsc, colsCsc, c3, nc);
    int triangles = 0;
    for (int i = 0; i < nc; i++)
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

    int *rowsCoo, *colsCoo, nr, nc, nnz;

    readMtxFile(argv[1], &rowsCoo, &colsCoo, &nr, &nc, &nnz);

    int *c3 = (int *)malloc(nc * sizeof(int));
    zeroOutArray(c3, nc);
    cooSequential(rowsCoo, colsCoo, c3, nnz, nc);
    int triangles = 0;
    for (int i = 0; i < nc; i++)
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
