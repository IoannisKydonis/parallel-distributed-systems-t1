#include <stdint.h>

#ifndef PRESENTER_H
#define PRESENTER_H

void runAndPresentResult(uint32_t *rowsCsc, uint32_t *colsCsc, uint32_t nc, void (* runnable) (uint32_t *, uint32_t *, uint32_t *, uint32_t), char *name, char *outputFilename, char *resultsFilename);

#endif //PRESENTER_H
