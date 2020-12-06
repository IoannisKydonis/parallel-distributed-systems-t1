#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <stdint.h>
#include <cilk/cilk.h>
#include <pthread.h>
#include "readmtx.h" // readMtxFile
#include "coo2csc.h" // coo2csc
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray
#include "controller.h"

pthread_mutex_t mutex;

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
            pthread_mutex_lock(&mutex);
            c3[colIndex1]++;
            c3[colIndex2]++;
            c3[arr[k]]++;
            pthread_mutex_unlock(&mutex);
        }
    }
}

// V3 Parallel with Cilk
void cscParallelCilk(uint32_t *rowsCsc, uint32_t *colsCsc, uint32_t *c3, uint32_t nc) {
    cilk_for (uint32_t i = 0; i < nc; i++) {
        for (uint32_t j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            uint32_t subRow = rowsCsc[j];
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

    uint32_t *rowsCoo, *colsCoo, nr, nc, nnz;

    readMtxFile(argv[1], &rowsCoo, &colsCoo, &nr, &nc, &nnz);

    uint32_t *rowsCsc = (uint32_t *)malloc(nnz * sizeof(uint32_t));
    uint32_t *colsCsc = (uint32_t *)malloc((nc + 1) * sizeof(uint32_t));
    coo2csc(rowsCsc, colsCsc, rowsCoo, colsCoo, nnz, nc, 0);

    pthread_mutex_init(&mutex,NULL);
    runAndPresentResult(rowsCsc, colsCsc, nc, cscParallelCilk, "V3 Cilk", "./v3-cilk.txt", "./v3-cilk-results.txt");

    free(rowsCoo);
    free(colsCoo);
    free(rowsCsc);
    free(colsCsc);
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}
