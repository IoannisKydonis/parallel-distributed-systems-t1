#ifndef ARRAYUTILS_H
#define ARRAYUTILS_H

int binarySearch(int *arr, int start, int end, int target);

void zeroOutArray(int *arr, int length);

void printArray(int *arr, int length);

void mergeArrays(int *arr1, int *arr2, int *res, int length1, int length2);

int countCommonElementsInSortedArrays(int *arr1, int *arr2, int length1, int length2);

void cscMatrixVectorMultiplication(int *row, int *col, int *vector, int *res, int nc);

int cmp(const void *a, const void *b);

void cscMaskedMatrixSquare(int *row, int *col, int *res, int nc);

#endif //ARRAYUTILS_H
