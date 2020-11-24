#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "readmtx.h"
#include "coo2csc.h"

using namespace std;

void cooSequential(int *rowsCoo, int *colsCoo, int nnz, int nc) {
    int triangleCount = 0;
    int **adj = (int **) malloc(nc * sizeof(int *));
    for (int i = 0; i < nc; i++)
        adj[i] = (int *) malloc(nc * sizeof(int));
    for (int i = 0; i < nc; i++)
        for (int j = 0; j < nc; j++)
            adj[i][j] = 0;
    for (int i = 0; i < nnz; i++) {
        adj[colsCoo[i]][rowsCoo[i]] = 1;
        adj[rowsCoo[i]][colsCoo[i]] = 1;
    }
    for (int i = 0; i < nc; i++) {
        for (int j = i + 1; j < nc; j++) {
            for (int k = j + 1; k < nc; k++) {
                if (adj[i][j] == 1 && adj[j][k] == 1 && adj[k][i] == 1)
                    triangleCount++;
            }
        }
    }
    for (int i = 0; i < nc; i++)
        free(adj[i]);
    free(adj);
    cout << "COO Triangles: " << triangleCount << endl;
}

void cscSequential(int *rowsCsc, int *colsCsc, int nc) {
    unordered_map<int, int> rowIndexes;
    unordered_map<int, int> rowSizes;
    for (int i = 0; i < nc; i++) {
        if (colsCsc[i] < colsCsc[i + 1]) {
            rowIndexes[i] = colsCsc[i];
            rowSizes[i] = colsCsc[i + 1] - colsCsc[i];
        }
    }

    unordered_map<int, unordered_set<int>> colsPerRow;
    for (int i = 0; i < nc; i++) {
        int rowStart = rowIndexes[i];
        int rowEnd = rowStart + rowSizes[i];
        unordered_set<int> set;
//        cout << "+++++++++++ " << "Row " << i << " > ";
        for (int j = rowStart; j < rowEnd; j++) {
            // Do not insert self loops
            if (i != rowsCsc[j]) {
                set.insert(rowsCsc[j]);
//                cout << rowsCsc[j] << " ";
            }
        }
//        cout << endl;
        colsPerRow[i] = set;
    }

    int triangleCount = 0;
    for (int i = 0; i < nc; i++) {
        unordered_set<int> rowElements = colsPerRow[i];
        for (int el : rowElements) {
            unordered_set<int> childElements = colsPerRow[el];
            for (int el2 : childElements) {
                if (rowElements.count(el2)) {
                    triangleCount++;
                }
            }
        }
    }
    cout << "Triangles: " << triangleCount << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [matrix-market-filename]\n", argv[0]);
        exit(1);
    }

    int *rowsCoo, *colsCoo, nr, nc, nnz;

    readMtxFile(argv[1], &rowsCoo, &colsCoo, &nr, &nc, &nnz);

    int *rowsCsc = (int *)malloc(nnz * sizeof(int));
    int *colsCsc = (int *)malloc((nc + 1) * sizeof(int));
    coo2csc(rowsCsc, colsCsc, rowsCoo, colsCoo, nnz, nc, 0);

    cooSequential(rowsCoo, colsCoo, nnz, nc);
    free(rowsCoo);
    free(colsCoo);

    cscSequential(rowsCsc, colsCsc, nc);
    free(rowsCsc);
    free(colsCsc);
    return 0;
}