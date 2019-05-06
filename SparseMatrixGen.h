#pragma once


#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <string.h>
#include <time.h>      
#include <stdint.h>
#include "mmio.h"


struct sparseMatrix
{
    int ** matrix;
    uint16_t rows;
    uint16_t cols;
    uint32_t size;

    void print()
    {
        printf("[");
        for( int row = 0 ; row < rows ; ++row )
        {
            printf("[");
            for( int col = 0 ; col < cols ; ++col )
            {
                printf("%d, ", matrix[row][col]);
            }
            printf("]\n");
        }
        printf("]\n");
    }
};

struct coordSparseMatrix
{
    double * values;
    uint16_t * rows;
    uint16_t * cols;
    uint16_t nonZero;
    uint32_t dimension;
    uint32_t size;

    void print()
    {
        MM_typecode matcode;

        mm_initialize_typecode(&matcode);
        mm_set_matrix(&matcode);
        mm_set_coordinate(&matcode);
        mm_set_real(&matcode);
        mm_write_banner(stdout, matcode);
        mm_write_mtx_crd_size(stdout, dimension, dimension, nonZero);

        for( int i = 0 ; i < nonZero ; ++i )
        {
            fprintf(stdout, "%d %d %10.3g\n", rows[i] + 1, cols[i] + 1, values[i]);
        }
    }
    void toFile(FILE * pFile)
    {
        if(pFile == NULL)
            return;
        
        MM_typecode matcode;

        mm_initialize_typecode(&matcode);
        mm_set_matrix(&matcode);
        mm_set_coordinate(&matcode);
        mm_set_real(&matcode);
        mm_write_banner(pFile, matcode);
        mm_write_mtx_crd_size(pFile, dimension, dimension, nonZero);

        for( int i = 0 ; i < nonZero ; ++i )
        {
            fprintf(pFile, "%d %d %10.3g\n", rows[i] + 1, cols[i] + 1, values[i]);
        }
    }

    ~coordSparseMatrix()
    {
        free( values );
        free( rows );
        free( cols );
    }
};

sparseMatrix genMatrix(uint16_t percentZero, bool diagonal, int minDim, int maxDim);

coordSparseMatrix genCoordMatrix(uint16_t percentZero, bool diagonal, int minDim, int maxDim);