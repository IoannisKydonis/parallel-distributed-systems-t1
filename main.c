#include <stdio.h>
#include <stdlib.h>
#include "readmtx.h"
#include "coo2csc.h"

int binarySearch(int *arr, int start, int end, int target) {
    while (start <= end) {
        int middle = (start + end) / 2;
        if (arr[middle] < target)
            start = middle + 1;
        else if (arr[middle] > target)
            end = middle - 1;
        else
            return 1;
    }
    return 0;
}

void swapInts(int *n1, int *n2) {
    int temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

int commonValueCountInSubarrays(int *arr, int start1, int end1, int start2, int end2) {
    int count = 0;
    if (end1 - start1 < end2 - start2) {
        swapInts(&start1, &start2);
        swapInts(&end1, &end2);
    }
    for (int k = start2; k < end2; k++) {
        if (k == arr[k])
            continue;
        if (binarySearch(arr, start1, end1 - 1, arr[k]))
            count++;
    }
    return count;
}

int cooSequential(int *rowsCoo, int *colsCoo, int nnz, int nc) {
    int triangleCount = 0;
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
                if (adj[i][j] == 1 && adj[j][k] == 1 && adj[k][i] == 1)
                    triangleCount++;
    for (int i = 0; i < nc; i++)
        free(adj[i]);
    free(adj);
    return triangleCount;
}

// V3 Sequential
int cscSequential(int *rowsCsc, int *colsCsc, int nc) {
    int triangleCount = 0;
    for (int i = 0; i < nc; i++) {
        for (int j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            int subRow = rowsCsc[j];
            if (i != subRow)
                triangleCount += commonValueCountInSubarrays(rowsCsc, colsCsc[subRow], colsCsc[subRow + 1], j + 1, colsCsc[i + 1]);
        }
    }
    return triangleCount;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [matrix-market-filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int *rowsCoo, *colsCoo, nr, nc, nnz;

    readMtxFile(argv[1], &rowsCoo, &colsCoo, &nr, &nc, &nnz);

    int *rowsCsc = (int *)malloc(nnz * sizeof(int));
    int *colsCsc = (int *)malloc((nc + 1) * sizeof(int));
    coo2csc(rowsCsc, colsCsc, rowsCoo, colsCoo, nnz, nc, 0);

    int cooResult = cooSequential(rowsCoo, colsCoo, nnz, nc);
    printf("COO Triangles: %d\n", cooResult);
    free(rowsCoo);
    free(colsCoo);

    int cscResult = cscSequential(rowsCsc, colsCsc, nc);
    printf("CSC Triangles: %d\n", cscResult);
    free(rowsCsc);
    free(colsCsc);
    return EXIT_SUCCESS;
}