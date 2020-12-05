#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <pthread.h>
#include "readmtx.h" // readMtxFile
#include "coo2csc.h" // coo2csc
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray
#define NUM_THREADS	12

struct Arg {
    int *row;
    int *col;
    int *res;
    int nc;
    int *colSizes;
    int **symmetricRowItems;
    int executeFrom;
    int executeTo;
};

pthread_mutex_t mutex;

void *multiplyMatrices(void *argument) {
    struct Arg a = *(struct Arg *) argument;

    for (int i = a.executeFrom; i < a.executeTo; i++) {
        for (int j = a.col[i]; j < a.col[i + 1]; j++) {
            int curRow = i;
            int curCol = a.row[j];
            if (curRow == curCol)
                continue;
            int fullRowSize = a.col[curRow + 1] - a.col[curRow] + a.colSizes[curRow];
            int *fullRow = (int *)malloc(fullRowSize * sizeof(int));
            mergeArrays(a.row + a.col[curRow], a.symmetricRowItems[curRow], fullRow, a.col[curRow + 1] - a.col[curRow], a.colSizes[curRow]);
            int fullColSize = a.col[curCol + 1] - a.col[curCol] + a.colSizes[curCol];
            int *fullCol = (int *)malloc(fullColSize * sizeof(int));
            mergeArrays(a.row + a.col[curCol], a.symmetricRowItems[curCol], fullCol, a.col[curCol + 1] - a.col[curCol], a.colSizes[curCol]);
            int sum = countCommonElementsInSortedArrays(fullRow, fullCol, fullRowSize, fullColSize);
            a.res[j] = sum;
            free(fullRow);
            free(fullCol);
        }
    }
}

void cscMaskedMatrixSquare(int *row, int *col, int *res, int nc) {
    pthread_t threads[NUM_THREADS];

    int *colSizes = (int *)malloc(nc * sizeof(int));
    zeroOutArray(colSizes, nc);
    for (int i = 0; i < nc; i++) {
        for (int j = col[i]; j < col[i + 1]; j++) {
            colSizes[row[j]]++;
        }
    }

    int *colIndexes = (int *)malloc(nc * sizeof(int));
    zeroOutArray(colIndexes, nc);
    int **symmetricRowItems = (int **)malloc(nc * sizeof(int *));
    for (int i = 0; i < nc; i++)
        symmetricRowItems[i] = (int *)malloc(colSizes[i] * sizeof(int));
    for (int i = 0; i < nc; i++) {
        for (int j = col[i]; j < col[i + 1]; j++) {
            symmetricRowItems[row[j]][colIndexes[row[j]]] = i;
            colIndexes[row[j]]++;
        }
    }

    // Multiply
    for (int i = 0; i < NUM_THREADS; i++) {
        int executeFrom = i * nc / NUM_THREADS;
        int executeTo = (i + 1) * nc / NUM_THREADS;
        struct Arg *a = malloc(sizeof(struct Arg));
        (*a).row = row;
        (*a).col = col;
        (*a).res = res;
        (*a).nc = nc;
        (*a).colSizes = colSizes;
        (*a).symmetricRowItems = symmetricRowItems;
        (*a).executeFrom = executeFrom;
        (*a).executeTo = executeTo;
        pthread_create(&threads[i], NULL, multiplyMatrices, (void *) a);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < nc; i++)
        free(symmetricRowItems[i]);
    free(symmetricRowItems);
    free(colIndexes);
    free(colSizes);
}

// V4 Parallel with pthreads
void cscParallelV4Pthreads(int *rowsCsc, int *colsCsc, int *c3, int nc) {
    int *res = (int *)malloc(colsCsc[nc] * sizeof(int));
    cscMaskedMatrixSquare(rowsCsc, colsCsc, res, nc);
    for (int i = 0; i < nc; i++) {
        for (int j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            c3[i] += res[j];
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

    runAndPresentResult(rowsCsc, colsCsc, nc, cscParallelV4Pthreads, "V4 Parallel pthreads");

    free(rowsCoo);
    free(colsCoo);
    free(rowsCsc);
    free(colsCsc);
    pthread_exit(NULL);
}
