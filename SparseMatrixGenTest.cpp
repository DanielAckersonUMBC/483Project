#include "SparseMatrixGen.h"

int main(int argc, char ** argv)
{
    // Seed the random number generator for this run.
    srand(time(NULL));

    for( int i = 0 ; i < 40 ; ++i )
    {
        sparseMatrix myMatrix = genMatrix(80, true, 80, 500);
        myMatrix.print();
    }
    
    return 0;
}