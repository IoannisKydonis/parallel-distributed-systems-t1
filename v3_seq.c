#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <stdint.h>
#include "readmtx.h" // readMtxFile
#include "coo2csc.h" // coo2csc
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray

void swapInts(uint32_t *n1, uint32_t *n2) {
    uint32_t temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

void checkForCommonValueInSubarrays(uint32_t *arr, uint32_t start1, uint32_t end1, uint32_t start2, uint32_t end2, uint32_t *c3, uint32_t colIndex1, uint32_t colIndex2) {
    if (end1 - start1 < end2 - start2) {
        swapInts(&start1, &start2);
        swapInts(&end1, &end2);
    }
    for (uint32_t k = start2; k < end2; k++) {
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

// V3 Sequential
void cscSequential(uint32_t *rowsCsc, uint32_t *colsCsc, uint32_t *c3, uint32_t nc) {
    for (uint32_t i = 0; i < nc; i++) {
        for (uint32_t j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            uint32_t subRow = rowsCsc[j];
            if (i != subRow)
                checkForCommonValueInSubarrays(rowsCsc, colsCsc[subRow], colsCsc[subRow + 1], j + 1, colsCsc[i + 1], c3, subRow, i);
        }
    }
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

    uint32_t *rowsCsc = (uint32_t *)malloc(nnz * sizeof(uint32_t));
    uint32_t *colsCsc = (uint32_t *)malloc((nc + 1) * sizeof(uint32_t));
    coo2csc(rowsCsc, colsCsc, rowsCoo, colsCoo, nnz, nc, 0);


    runAndPresentResult(rowsCsc, colsCsc, nc, cscSequential, "V3 Sequential");

    free(rowsCoo);
    free(colsCoo);
    free(rowsCsc);
    free(colsCsc);
    return EXIT_SUCCESS;
}

