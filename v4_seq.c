#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include "readmtx.h" // readMtxFile
#include "coo2csc.h" // coo2csc
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray

// V4 Sequential
void cscSequentialV4(int *rowsCsc, int *colsCsc, int *c3, int nc) {
    int *res = (int *)malloc(colsCsc[nc] * sizeof(int));
    cscMaskedMatrixSquare(rowsCsc, colsCsc, res, nc);
    for (int i = 0; i < nc; i++) {
        for (int j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            c3[i] += res[j];
        }
    }
    // TODO: kydonis - get 2 versions of csc, one normal and one with reversed col* row* arrays int coo2csc -> merge them
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

    runAndPresentResult(rowsCsc, colsCsc, nc, cscSequentialV4, "V4 Sequential");

    free(rowsCoo);
    free(colsCoo);
    free(rowsCsc);
    free(colsCsc);
    return EXIT_SUCCESS;
}
