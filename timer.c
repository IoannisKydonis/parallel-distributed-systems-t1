#include <time.h> // clock_gettime, timespec

double measureTimeForRunnable(void (* runnable) (int *, int *, int *, int), int *arg1, int *arg2, int *arg3, int arg4) {
    struct timespec ts_start;
    struct timespec ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    runnable(arg1, arg2, arg3, arg4);
    clock_gettime(CLOCK_MONOTONIC, &ts_end);
    return (ts_end.tv_sec - ts_start.tv_sec) + (ts_end.tv_nsec - ts_start.tv_nsec) / 1000000000.0;
}
