#include <stdio.h> // fprintf, printf
#include <stdlib.h> // malloc

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

void zeroOutArray(int *arr, int length) {
    for (int i = 0; i < length; i++)
        arr[i] = 0;
}

void printArray(int *arr, int length) {
    for (int i = 0; i < length; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void mergeArrays(int *arr1, int *arr2, int *res, int length1, int length2) {
    int index1 = 0;
    int index2 = 0;
    while (index1 < length1 && index2 < length2) {
        if (arr1[index1] < arr2[index2]) {
            res[index1 + index2] = arr1[index1];
            index1++;
        } else {
            res[index1 + index2] = arr2[index2];
            index2++;
        }
    }
    while (index1 < length1) {
        res[index1 + index2] = arr1[index1];
        index1++;
    }
    while (index2 < length2) {
        res[index1 + index2] = arr2[index2];
        index2++;
    }
}

int countCommonElementsInSortedArrays(int *arr1, int *arr2, int length1, int length2) {
    int sum = 0;
    int index1 = 0;
    int index2 = 0;
    while (index1 < length1 && index2 < length2) {
        if (arr1[index1] < arr2[index2]) {
            index1++;
        } else if (arr1[index1] > arr2[index2]) {
            index2++;
        } else {
            sum++;
            index1++;
            index2++;
        }
    }
    return sum;
}

void cscMatrixVectorMultiplication(int *row, int *col, int *vector, int *res, int nc) {
    for (int i = 0; i < nc; i++) {
        res[i] = 0;
        for (int j = col[i]; j < col[i + 1]; j++)
            res[i] += vector[row[j]];
    }
}

int cmp(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

//void cscMaskedMatrixSquare(int *row, int *col, int *res, int nc) {
//    int *colSizes = (int *)malloc(nc * sizeof(int));
//    zeroOutArray(colSizes, nc);
//    for (int i = 0; i < nc; i++) {
//        for (int j = col[i]; j < col[i + 1]; j++) {
//            colSizes[row[j]]++;
//        }
//    }
//
//    int *colIndexes = (int *)malloc(nc * sizeof(int));
//    zeroOutArray(colIndexes, nc);
//    int **symmetricRowItems = (int **)malloc(nc * sizeof(int *));
//    for (int i = 0; i < nc; i++)
//        symmetricRowItems[i] = (int *)malloc(colSizes[i] * sizeof(int));
//    for (int i = 0; i < nc; i++) {
//        for (int j = col[i]; j < col[i + 1]; j++) {
//            symmetricRowItems[row[j]][colIndexes[row[j]]] = i;
//            colIndexes[row[j]]++;
//        }
//    }
//
//    // Multiply
//    for (int i = 0; i < nc; i++) {
//        for (int j = col[i]; j < col[i + 1]; j++) {
//            int curRow = i;
//            int curCol = row[j];
//            if (curRow == curCol)
//                continue;
//            int fullRowSize = col[curRow + 1] - col[curRow] + colSizes[curRow];
//            int *fullRow = (int *)malloc(fullRowSize * sizeof(int));
//            mergeArrays(row + col[curRow], symmetricRowItems[curRow], fullRow, col[curRow + 1] - col[curRow], colSizes[curRow]);
//            int fullColSize = col[curCol + 1] - col[curCol] + colSizes[curCol];
//            int *fullCol = (int *)malloc(fullColSize * sizeof(int));
//            mergeArrays(row + col[curCol], symmetricRowItems[curCol], fullCol, col[curCol + 1] - col[curCol], colSizes[curCol]);
//            int sum = countCommonElementsInSortedArrays(fullRow, fullCol, fullRowSize, fullColSize);
//            res[j] = sum;
//            free(fullRow);
//            free(fullCol);
//        }
//    }
//
//    for (int i = 0; i < nc; i++)
//        free(symmetricRowItems[i]);
//    free(symmetricRowItems);
//    free(colIndexes);
//    free(colSizes);
//}
