#include "SparseMatrixGen.h"

int main(int argc, char ** argv)
{
    if (argc < 4) {
        printf("Usage: %s num_mat mat_dim pct_zero", argv[0]);
        return 1;
    }
    // Seed the random number generator for this run.
    srand(time(NULL));

    FILE * pFile = fopen("Matrices", "w");
    for( int i = 0 ; i < atoi(argv[1]) ; ++i )
    {
        coordSparseMatrix myMatrix = genCoordMatrix(atoi(argv[3]), true, atoi(argv[2]), 0, 1e12);
        /* printf("Matrix %d generated.\n", i); */
        myMatrix.toFile(pFile);
        /* printf("Matrix %d written.\n", i); */
    }
    fclose(pFile);
    return 0;
}
