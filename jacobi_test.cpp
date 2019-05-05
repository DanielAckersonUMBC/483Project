/*
 * jacobi linear solver w/o concurrency
 *
 * Based heavily on jacobi_prb.cpp from https://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi/jacobi.html
 * Direct link: https://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi/jacobi_prb.cpp
 */

# include <cstdlib>
# include <iostream>
# include <cmath>

using namespace std;

# include "jacobi.hpp"

int main()
{
    double *a;
    double *b;
    int i;
    int it;
    int it_num;
    int n;
    double t;
    double *x;
    double *x_exact;
    double *x_new;

    double w = 0.5;

    it_num = 2000;
    n = 33;

    //  Set the matrix A.
    a = dif2(n, n);

    //  Determine the right hand side vector B.
    x_exact = new double[n];
    for (i = 0; i < n; i++) {
        t = (double) i / (double) (n - 1);
        x_exact[i] = exp(t) * (t - 1) * t;
    }
    b = r8mat_mv_new(n, n, a, x_exact);

    //  Set the initial estimate for the solution.
    x = new double[n];
    for (i = 0; i < n; i++) {
        x[i] = 0.0;
    }

    //  Carry out the iteration.
    for (it = 1; it <= it_num; it++) {
        x_new = jacobi1(n, a, b, x);

        //  Update the solution
        for (i = 0; i < n; i++) {
            x[i] = (1.0 - w) * x[i] + w * x_new[i];
        }

        delete[] x_new;
    }

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

    return 0;
}
