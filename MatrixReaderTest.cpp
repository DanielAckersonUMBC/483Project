#include <unistd.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include "MatrixReader.h"

#include "jacobi.hpp"
#include "ParallelJacobiSolver.hpp"

char const optstring[] = "n:i:m:t:f:";
char const usagestring[] =
    "[-n numthreads]\n"
    "[-i max_iterations]\n"
    "[-m matrix_order]\n"
    "[-f filename]\n"
    "[-t threshold]\n";

int main(int argc, char *argv[])
{
    double threshold = 1e-10;
    std::string filename = "";
    int it_num = 2000;
    int n = 33;
    int nthreads = 4;
    int ret = 0;
    int opt;
    std::stringstream ss;

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        ss.clear();
        switch (opt) {
        case 'n':
            ss << optarg;
            ss >> nthreads;
            break;
        case 'i':
            ss << optarg;
            ss >> it_num;
            break;
        case 'm':
            ss << optarg;
            ss >> n;
            break;
        case 't':
            ss << optarg;
            ss >> threshold;
            break;
        case 'f':
            ss << optarg;
            ss >> filename;
            break;
        default:
            std::cout << "Usage: " << argv[0] << '\n' << usagestring << std::endl;
            return 0;
        }
    }
    if( filename == "" )
    {
        std::cout << "Usage: " << argv[0] << '\n' << usagestring << std::endl;
        return 0;
    }

    /* std::cout << "Using " << nthreads << " threads" << std::endl; */
    /* std::cout << "Using " << it_num << " max iterations" << std::endl; */

    ParallelJacobiSolver pjs(nthreads, it_num, threshold);

    MatrixReader mtxReader(filename.c_str());
    jacobiMatrix matrix = mtxReader.getMatrix();
    n = matrix.n; 

    // Solve
    SolutionStats solution = pjs.solve(matrix.n, matrix.a, matrix.b, matrix.x);

    /* std::cout << "Stopped after " << solution.iterations << " iterations" << std::endl; */
    /* std::cout << "Took " << solution.time_sec << " seconds" << std::endl; */
    /* std::cout << "Threshold = " << threshold << std::endl; */
    /* std::cout << "Norm: ||Xi+1 - Xi|| = " <<  solution.norm << std::endl; */

    std::cout
            <<  solution.time_sec <<
        ',' <<  solution.iterations <<
        ',' <<  solution.norm <<
        ',' <<  n <<
        ',' <<  nthreads <<
        ',' <<  threshold <<
        std::endl;

    /* double *x_err = new double[n]; */
    /* for (int i = 0; i < n; i++) { */
    /*     x_err[i] = matrix.x[i] - matrix.x_exact[i]; */
    /* } */

    /* r8vec_print(n, matrix.x, "Solution"); */
    /* r8vec_print(n, matrix.x_exact, "True Solution"); */
    /* r8vec_print(n, x_err, "Solution error"); */

    return ret;
}
