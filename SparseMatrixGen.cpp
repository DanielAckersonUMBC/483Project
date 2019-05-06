#include "SparseMatrixGen.h"
#include "stdlib.h"

sparseMatrix genMatrix(uint16_t percentZero, bool diagonal, int minDim, int maxDim)
{
    // Randomly determine number of rows and columns.
    uint16_t rows = rand() % (maxDim - minDim) + minDim;
    uint16_t cols = rows;

    // Dynamically allocate two dimmensional matrix.
    int ** matrix = new int*[rows];
    for( int i = 0 ; i < rows ; ++i )
        matrix[i] = new int[cols];

    // Fill the matrix
    for(int row = 0 ; row < rows ; ++row)
    {
        for( int col = 0 ; col < cols ; ++col )
        {
            // If a random number between 0-100 falls above percent zero value it will be made a random value.
            // Also if we are diagonalizing we check if it's on the diagonal and force it anyway.
            if( rand() % 100 > percentZero || (diagonal && row == col))
            {
                matrix[row][col] = rand() % 100 + 1;
            }
            else
            {
                // Otherwise we make it zero.
                matrix[row][col] = 0;
            }
        }
    }

    // Initialize, populate, and return the sparse matrix object.
    sparseMatrix sparseMat;

    sparseMat.matrix = matrix;
    sparseMat.rows = rows;
    sparseMat.cols = cols;
    sparseMat.size = rows * cols * sizeof(int);

    return sparseMat;
}

coordSparseMatrix genCoordMatrix(uint16_t percentZero, bool diagonal, int minDim, int maxDim)
{
    // Randomly determine number of rows and columns.
    uint16_t numRows = rand() % (maxDim - minDim) + minDim;
    uint16_t numCols = numRows;
    int64_t nonZero = 0;

    // Preallocated large enough arrays.
    //int allocationSize = numRows + ((1.0 - (percentZero/100.0) + .03) * (numRows * numCols));
    int allocationSize = numRows * numCols;
    double * tempVals = new double[allocationSize];
    uint16_t * tempRows = new uint16_t[allocationSize];
    uint16_t * tempCols = new uint16_t[allocationSize]; 

    if(tempVals == NULL || tempRows == NULL || tempCols == NULL )
    {
        printf( "Allocation failed!\n" );
        exit(1);
    }

    // Fill the matrix
    for(int row = 0 ; row < numRows ; ++row)
    {
        for( int col = 0 ; col < numCols ; ++col )
        {
            // If a random number between 0-100 falls above percent zero value it will be made a random value.
            // Also if we are diagonalizing we check if it's on the diagonal and force it anyway.
            if( (rand() % 100 > percentZero) || (diagonal && row == col))
            {
                nonZero++;

                // Add the value to the matrix.
                tempVals[nonZero - 1] = (double)rand() / (double)RAND_MAX;
                tempRows[nonZero - 1] = row;
                tempCols[nonZero - 1] = col;
            }
        }
    }
    double * values = new double[nonZero];
    uint16_t * rows = new uint16_t[nonZero];
    uint16_t * cols = new uint16_t[nonZero];


    // Copy over necessary values.
    memcpy(values, tempVals, sizeof(double) * nonZero);
    memcpy(rows, tempRows, sizeof(uint16_t) * nonZero);
    memcpy(cols, tempCols, sizeof(uint16_t) * nonZero);

    delete[] tempVals;
    delete[] tempRows;
    delete[] tempCols;

    // Initialize, populate, and return the sparse matrix object.
    coordSparseMatrix coordSparseMat;

    coordSparseMat.values = values;
    coordSparseMat.rows = rows;
    coordSparseMat.cols = cols;
    coordSparseMat.nonZero = nonZero;
    coordSparseMat.dimension = numRows;

    return coordSparseMat;
}