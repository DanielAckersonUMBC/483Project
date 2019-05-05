/*
 * jacobi linear solver with concurrency using pthreads
 *
 * Based heavily on jacobi_prb.cpp from https://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi/jacobi.html
 * Direct link: https://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi/jacobi_prb.cpp
 */

#include <unistd.h>
#include <cmath>
#include <iostream>
#include <sstream>

#include "jacobi.hpp"
#include "ParallelJacobiSolver.hpp"

char const optstring[] = "n:i:m:";

int main(int argc, char *argv[])
{
    double *a;
    double *b, *x, *x_exact;
    double t;
    int it_num = 2000;
    int n = 33; // matrix size
    int nthreads = 4;
    int ret = 0;
    int opt;
    std::stringstream ss;

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'n':
            ss.clear();
            ss << optarg;
            ss >> nthreads;
            break;
        case 'i':
            ss.clear();
            ss << optarg;
            ss >> it_num;
            break;
        case 'm':
            ss.clear();
            ss << optarg;
            ss >> n;
        default:
            std::cout << "Usage: " << argv[0] << " [-n numthreads] [-i max_iterations]" << std::endl;
        }
    }

    std::cout << "Using " << nthreads << " threads" << std::endl;
    std::cout << "Using " << it_num << " max iterations" << std::endl;

    //  Set the matrix A.
    a = dif2(n, n);

    //  Determine the right hand side vector B.
    x_exact = new double[n];
    for (int i = 0; i < n; i++) {
        t = (double) i / (double) (n - 1);
        x_exact[i] = exp(t) * (t - 1) * t;
    }
    b = r8mat_mv_new(n, n, a, x_exact);

    //  Set the initial estimate for the solution.
    x = new double[n];
    for (int i = 0; i < n; i++) {
        x[i] = 0.0;
    }

    // Solve
    ParallelJacobiSolver pjs(nthreads, it_num);
    SolutionStats solution = pjs.solve(n, a, b, x);
    if (solution.solved) {
        std::cout << "Solved!" << std::endl;
        std::cout << "Took " << solution.time_sec << " seconds" << std::endl;
    }
    std::cout << "Stopped after " << solution.iterations << " iterations" << std::endl;

    double *x_err = new double[n];
    for (int i = 0; i < n; i++) {
        x_err[i] = x[i] - x_exact[i];
    }

    r8vec_print(n, x, "Solution");
    r8vec_print(n, x_exact, "True Solution");
    r8vec_print(n, x_err, "Solution error");

    delete[] a;
    delete[] b;
    delete[] x;
    delete[] x_exact;

    return ret;
}
