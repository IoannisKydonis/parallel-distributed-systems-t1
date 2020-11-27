#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include "readmtx.h" // readMtxFile
#include "coo2csc.h" // coo2csc
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray

void swapInts(int *n1, int *n2) {
    int temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

void checkForCommonValueInSubarrays(int *arr, int start1, int end1, int start2, int end2, int *c3, int colIndex1, int colIndex2) {
    if (end1 - start1 < end2 - start2) {
        swapInts(&start1, &start2);
        swapInts(&end1, &end2);
    }
    for (int k = start2; k < end2; k++) {
        if (k == arr[k])
            continue;
        if (binarySearch(arr, start1, end1 - 1, arr[k])) {
#pragma omp atomic
                c3[colIndex1]++;
#pragma omp atomic
                c3[colIndex2]++;
#pragma omp atomic
                c3[arr[k]]++;
        }
    }
}

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

// V3 Sequential
void cscSequential(int *rowsCsc, int *colsCsc, int *c3, int nc) {
    for (int i = 0; i < nc; i++) {
        for (int j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            int subRow = rowsCsc[j];
            if (i != subRow)
                checkForCommonValueInSubarrays(rowsCsc, colsCsc[subRow], colsCsc[subRow + 1], j + 1, colsCsc[i + 1], c3, subRow, i);
        }
    }
}

// V3 Parallel with OpenMP
void cscParallelOmp(int *rowsCsc, int *colsCsc, int *c3, int nc) {
#pragma omp parallel for
    for (int i = 0; i < nc; i++) {
        for (int j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            int subRow = rowsCsc[j];
            if (i != subRow)
                checkForCommonValueInSubarrays(rowsCsc, colsCsc[subRow], colsCsc[subRow + 1], j + 1,colsCsc[i + 1], c3, subRow, i);
        }
    }
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
    coo2csc(rowsCsc, colsCsc, rowsCoo, colsCoo, nnz, nc, 0); // TODO: kydonis - check if unsigned ints are needed

    free(rowsCoo);
    free(colsCoo);

    int *c3Csc = (int *)malloc(nc * sizeof(int));
    zeroOutArray(c3Csc, nc);
    double sequentialCscTime = measureTimeForRunnable(cscSequential, rowsCsc, colsCsc, c3Csc, nc);

    int *c3CscParallel = (int *)malloc(nc * sizeof(int));
    zeroOutArray(c3CscParallel, nc);
    double parallelCscTime = measureTimeForRunnable(cscParallelOmp, rowsCsc, colsCsc, c3CscParallel, nc);

    int correct = 1;
    int totalTrianglesCsc = 0;
    int totalTrianglesCscParallel = 0;
    for (int i = 0; i < nc; i++) {
        if (c3Csc[i] != c3CscParallel[i])
            correct = 0;
        totalTrianglesCsc += c3Csc[i];
        totalTrianglesCscParallel += c3CscParallel[i];
    }
    printf("CSC - CSC Par Match: %s\n", correct ? "TRUE" : "FALSE");
    printf("CSC Triangles: %d\n", totalTrianglesCsc / 3);
    printf("CSC Parallel Triangles: %d\n", totalTrianglesCscParallel / 3);
    printf("CSC Time:          %04.6lf seconds\n", sequentialCscTime);
    printf("CSC Parallel Time: %04.6lf seconds\n", parallelCscTime);
    printf("Time difference:   %04.6lf seconds\n", sequentialCscTime - parallelCscTime);

    free(rowsCsc);
    free(colsCsc);
    free(c3Csc);
    free(c3CscParallel);
    return EXIT_SUCCESS;
}