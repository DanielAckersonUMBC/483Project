#include "MatrixReader.h"

MatrixReader::MatrixReader()
: m_pFile(NULL), m_filename(NULL)
{
}

MatrixReader::MatrixReader(const char * filename)
{
    m_pFile = fopen(filename, "r");
}

MatrixReader::MatrixReader(FILE * pFile)
{
    if( pFile != NULL )
        m_pFile = pFile;
    else
        m_pFile = NULL;
}


MatrixReader::~MatrixReader()
{
}

jacobiMatrix MatrixReader::getMatrix()
{
    jacobiMatrix retMatrix;
    if( m_pFile == NULL )
    {
        fprintf( stderr, "Error: No file has been opened for MatrixReader.\n");
        return retMatrix;
    }
    int retcode = 0;
    MM_typecode matcode;
    mm_read_banner(m_pFile, &matcode);
    int n, m, nonZero = 0;

    retcode = mm_read_mtx_crd_size( m_pFile, &n, &m, &nonZero );

    if( retcode != 0 )
    {
        fprintf(stdout, "End of file reached.\n");
        return retMatrix;
    }


    // Values to read each line into from file.
    int row;
    int col;
    double value;

    // One dimensional array to hold the matrix; indexed [row * col].
    double * matrix = new double[n*m];

    // For each line of non-zero values we have to get the row, column, and value.
    for( int i = 0 ; i < nonZero ; ++i)
    {
        fscanf(m_pFile, "%d %d %lg\n", &row, &col, &value);

        // Matrix Market uses 1 based indexing.
        row--; 
        col--;

        matrix[col + row * n] = value;
    }

    retMatrix.a = matrix;
    retMatrix.n = n;
    retMatrix.setSolution();

    return retMatrix;
}