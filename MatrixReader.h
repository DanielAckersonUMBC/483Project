#pragma once

#include "mmio.h"
#include "jacobi.hpp"
#include <cmath>
#include <stdio.h>

struct jacobiMatrix
{
    // Matrix.
    double * a;

    // Matrix dimension.
    int n;

    // Right hand side vector B.
    double * b;

    // Exact and estimated solutions.
    double * x;
    double * x_exact;

    void getSolution()
    {
        //  Determine the right hand side vector B.
        x_exact = new double[n];
        for (int i = 0; i < n; i++) {
            double t = (double) i / (double) (n - 1);
            x_exact[i] = exp(t) * (t - 1) * t;
        }
        b = r8mat_mv_new(n, n, a, x_exact);

        //  Set the initial estimate for the solution.
        x = new double[n];
        for (int i = 0; i < n; i++) {
            x[i] = 0.0;
        }
    }
};

class MatrixReader 
{
public:

    MatrixReader();
    MatrixReader(const char * filename);
    MatrixReader(FILE * pFile);

    ~MatrixReader();

    double * getMatrix();
private:
    char * m_filename;
    FILE * m_pFile;

};
