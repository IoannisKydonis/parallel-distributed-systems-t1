#include <stdio.h> // fprintf, printf

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