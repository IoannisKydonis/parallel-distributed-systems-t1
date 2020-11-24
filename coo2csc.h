//
// Created by Ioannis Kydonis on 23/11/20.
//

#ifndef COO2CSC_H
#define COO2CSC_H

/* This ifdef allows the header to be used from both C and C++. */
#ifdef __cplusplus
extern "C" {
#endif

void coo2csc(
        int       * const row,       /*!< CSC row start indices */
        int       * const col,       /*!< CSC column indices */
        int const * const row_coo,   /*!< COO row indices */
        int const * const col_coo,   /*!< COO column indices */
        int const         nnz,       /*!< Number of nonzero elements */
        int const         n,         /*!< Number of rows/columns */
        int const         isOneBased /*!< Whether COO is 0- or 1-based */
);

#ifdef __cplusplus
}
#endif

#endif
