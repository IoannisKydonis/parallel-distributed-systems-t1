#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <cilk/cilk.h>
#include <pthread.h>
#include "readmtx.h" // readMtxFile
#include "coo2csc.h" // coo2csc
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray

pthread_mutex_t mutex;

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
            pthread_mutex_lock(&mutex);
            c3[colIndex1]++;
            c3[colIndex2]++;
            c3[arr[k]]++;
            pthread_mutex_unlock(&mutex);
        }
    }
}

// V3 Parallel with Cilk
void cscParallelCilk(int *rowsCsc, int *colsCsc, int *c3, int nc) {
    cilk_for (int i = 0; i < nc; i++) {
        for (int j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            int subRow = rowsCsc[j];
            if (i != subRow)
                checkForCommonValueInSubarrays(rowsCsc, colsCsc[subRow], colsCsc[subRow + 1], j + 1,colsCsc[i + 1], c3, subRow, i);
        }
    }
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

    int *rowsCsc = (int *)malloc(nnz * sizeof(int));
    int *colsCsc = (int *)malloc((nc + 1) * sizeof(int));
    coo2csc(rowsCsc, colsCsc, rowsCoo, colsCoo, nnz, nc, 0); // TODO: kydonis - check if unsigned ints are needed

    pthread_mutex_init(&mutex,NULL);
    runAndPresentResult(rowsCsc, colsCsc, nc, cscParallelCilk, "V3 Cilk");

    free(rowsCoo);
    free(colsCoo);
    free(rowsCsc);
    free(colsCsc);
    return EXIT_SUCCESS;
}
