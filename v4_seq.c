#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <stdint.h>
#include "readmtx.h" // readMtxFile
#include "coo2csc.h" // coo2csc
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray
#include "controller.h"

// V4 Sequential
void cscSequentialV4(uint32_t *row, uint32_t *col, uint32_t *res, uint32_t nc) {
    uint32_t *colSizes = (uint32_t *)malloc(nc * sizeof(uint32_t));
    zeroOutArray(colSizes, nc);
    for (uint32_t i = 0; i < nc; i++) {
        for (uint32_t j = col[i]; j < col[i + 1]; j++) {
            colSizes[row[j]]++;
        }
    }

    // Calculate symmetric csc values
    uint32_t *colIndexes = (uint32_t *)malloc(nc * sizeof(uint32_t));
    zeroOutArray(colIndexes, nc);
    uint32_t **symmetricRowItems = (uint32_t **)malloc(nc * sizeof(uint32_t *));
    for (uint32_t i = 0; i < nc; i++)
        symmetricRowItems[i] = (uint32_t *) malloc(colSizes[i] * sizeof(uint32_t));
    for (uint32_t i = 0; i < nc; i++) {
        for (uint32_t j = col[i]; j < col[i + 1]; j++) {
            symmetricRowItems[row[j]][colIndexes[row[j]]] = i;
            colIndexes[row[j]]++;
        }
    }

    // Multiply
    for (uint32_t i = 0; i < nc; i++) {
        for (uint32_t j = col[i]; j < col[i + 1]; j++) {
            uint32_t curRow = i;
            uint32_t curCol = row[j];
            if (curRow == curCol)
                continue;
            uint32_t fullRowSize = col[curRow + 1] - col[curRow] + colSizes[curRow];
            uint32_t *fullRow = (uint32_t *)malloc(fullRowSize * sizeof(uint32_t));
            mergeArrays(row + col[curRow], symmetricRowItems[curRow], fullRow, col[curRow + 1] - col[curRow], colSizes[curRow]);
            uint32_t fullColSize = col[curCol + 1] - col[curCol] + colSizes[curCol];
            uint32_t *fullCol = (uint32_t *)malloc(fullColSize * sizeof(uint32_t));
            mergeArrays(row + col[curCol], symmetricRowItems[curCol], fullCol, col[curCol + 1] - col[curCol], colSizes[curCol]);
            uint32_t sum = countCommonElementsInSortedArrays(fullRow, fullCol, fullRowSize, fullColSize);
            res[curCol] += sum;
            res[curRow] += sum;
            free(fullRow);
            free(fullCol);
        }
    }
    for (uint32_t i = 0; i < nc; i++) {
        res[i] /= 2;
    }
    for (uint32_t i = 0; i < nc; i++)
        free(symmetricRowItems[i]);
    free(symmetricRowItems);
    free(colIndexes);
    free(colSizes);
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

    runAndPresentResult(rowsCsc, colsCsc, nc, cscSequentialV4, "V4 Sequential", "./v4-seq.txt", "./v4-seq-results.txt");

    free(rowsCoo);
    free(colsCoo);
    free(rowsCsc);
    free(colsCsc);
    return EXIT_SUCCESS;
}
