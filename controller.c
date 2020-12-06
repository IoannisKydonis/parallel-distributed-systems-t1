#include <stdio.h> // fprintf, printf
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <stdint.h>
#include "timer.h" // measureTimeForRunnable
#include "arrayutils.h" // binarySearch, zeroOutArray, printArray

void saveOutput(uint32_t *results, uint32_t size, char *filename) {
    FILE *f = fopen(filename, "wb");
    for (uint32_t i = 0; i < size; i++) {
        fprintf(f, "%d ", results[i]);
    }
    fclose(f);
}

void saveStats(char *name, double time, uint32_t triangles, char *filename) {
    FILE *f = fopen(filename, "wb");
    fprintf(f, "Algorithm: %s\n", name);
    fprintf(f, "Time: %lf sec\n", time);
    fprintf(f, "Triangles: %d", triangles);
    fclose(f);
}

void runAndPresentResult(uint32_t *rowsCsc, uint32_t *colsCsc, uint32_t nc, void (* runnable) (uint32_t *, uint32_t *, uint32_t *, uint32_t), char *name, char *outputFilename, char *resultsFilename) {
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
    saveOutput(c3, nc, outputFilename);
    saveStats(name, time, triangles, resultsFilename);
    free(c3);
}
