#include "MatrixReader.h"

int main()
{
    MatrixReader mtxReader("Matricies");

    double * matrix = mtxReader.getMatrix();

    return 0;
}