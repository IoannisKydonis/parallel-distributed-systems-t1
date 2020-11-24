//
// Created by Ioannis Kydonis on 23/11/20.
//
#ifndef READMTX_H
#define READMTX_H

/* This ifdef allows the header to be used from both C and C++. */
#ifdef __cplusplus
extern "C" {
#endif

int readMtxFile(char *filename, int **rows, int **columns, int *rowCount, int *colCount, int *nnz);

#ifdef __cplusplus
}
#endif

#endif
