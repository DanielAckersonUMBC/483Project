#include "SparseMatrixGen.h"


int main(int argc, char ** argv)
{
    // Seed the random number generator for this run.
    srand(time(NULL));

    FILE * pFile = fopen("Matricies", "w");
    for( int i = 0 ; i < 1 ; ++i )
    {
        coordSparseMatrix myMatrix = genCoordMatrix(80, true, 800, 1000);
        printf("Matrix %d generated.\n", i);
        myMatrix.toFile(pFile);
        printf("Matrix %d written.\n", i);
    }
    fclose(pFile);
    return 0;
}