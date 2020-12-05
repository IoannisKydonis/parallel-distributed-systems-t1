#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include "readmtx.h" // readMtxFile
#include "coo2csc.h" // coo2csc
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray

// V4 Sequential
void cscSequentialV4(uint32_t *rowsCsc, uint32_t *colsCsc, uint32_t *c3, uint32_t nc) {
    uint32_t *res = (uint32_t *)malloc(colsCsc[nc] * sizeof(uint32_t));
    cscMaskedMatrixSquare(rowsCsc, colsCsc, res, nc);
    for (uint32_t i = 0; i < nc; i++) {
        for (uint32_t j = colsCsc[i]; j < colsCsc[i + 1]; j++) {
            c3[i] += res[j];
        }
    }
    // TODO: kydonis - get 2 versions of csc, one normal and one with reversed col* row* arrays int coo2csc -> merge them
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

    runAndPresentResult(rowsCsc, colsCsc, nc, cscSequentialV4, "V4 Sequential");

    free(rowsCoo);
    free(colsCoo);
    free(rowsCsc);
    free(colsCsc);
    return EXIT_SUCCESS;
}
