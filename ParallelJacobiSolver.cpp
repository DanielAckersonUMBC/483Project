/*
 * jacobi linear solver with concurrency using pthreads
 *
 * Based on jacobi.cpp from https://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi/jacobi.html
 * Direct link: https://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi/jacobi.cpp
 */

#include "ParallelJacobiSolver.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <pthread.h>


ParallelJacobiSolver::ParallelJacobiSolver(int nthreads, int max_iterations, double threshold)
    :   nthreads(nthreads),
        max_iterations(max_iterations),
        threshold(fabs(threshold))
{ }


void ParallelJacobiSolver::setThreads(int n)
{
    nthreads = n;
}


int ParallelJacobiSolver::getThreads()
{
    return nthreads;
}


void ParallelJacobiSolver::setMaxIterations(int i)
{
    max_iterations = i;
}


int ParallelJacobiSolver::getMaxIterations()
{
    return max_iterations;
}


void ParallelJacobiSolver::setSolutionThreshold(double t)
{
    threshold = fabs(t);
}


double ParallelJacobiSolver::getSolutionThreshold()
{
    return threshold;
}


struct SolverThread {
    pthread_t job;
    pthread_barrier_t *barrier;
    double const *a, *b;
    double *x, *x_new;
    double threshold;
    int begin, end;
    int n;
    int id;
    bool volatile *quit;
    bool volatile converged;
};


static void swap_buffer(void **b1, void **b2)
{
    void *temp = *b1;
    *b1 = *b2;
    *b2 = temp;
}


static void *JacobiRoutine(void *arg)
{
    SolverThread *t = (SolverThread *) arg;
    bool converged = true;

    /* std::cout << "thread " << t->id << " is waiting" << std::endl; */
    pthread_barrier_wait(t->barrier); // wait for main thread
    /* std::cout << "thread " << t->id << " is going" << std::endl; */
    while (! *t->quit) {
        /* std::cout << "thread " << t->id << " is calculating range (" << t->begin << ", " << t->end << ")" << std::endl; */
        for (int i = t->begin; i < t->end; i++) {
            t->x_new[i] = t->b[i];
            for (int j = 0; j < t->n; j++) {
                if (i != j)
                    t->x_new[i] -= t->a[i + j * t->n] * t->x[j];
            }
            t->x_new[i] /= t->a[i + i * t->n];
            if (fabs(t->x_new[i] - t->x[i]) > t->threshold)
                converged = false;
        }

        t->converged = converged;
        swap_buffer((void **) &t->x, (void **) &t->x_new);

        pthread_barrier_wait(t->barrier); // signal main thread
        pthread_barrier_wait(t->barrier); // wait for main thread
    }

    /* std::cout << "thread " << t->id << " is exiting" << std::endl; */
    return NULL;
}


SolutionStats ParallelJacobiSolver::solve(int n, double const a[], double const b[], double x[])
{
    SolutionStats stats;
    SolverThread threads[n];
    pthread_barrier_t barrier;
    double *x_local = new double[n];
    double *x_new = new double[n];
    bool volatile quit = false;

    pthread_barrier_init(&barrier, NULL, nthreads + 1);
    memcpy(x_local, x, n * sizeof (double));
    stats.solved = false;

    int const chunksize = (n + nthreads - 1) / nthreads;
    for (int i = 0; i < nthreads; i++) {
        SolverThread *t = &threads[i];
        int const begin = i * chunksize;
        int const end = begin + chunksize;

        t->id = i;
        t->barrier = &barrier;
        t->begin = begin;
        t->end = (n < end)? n : end;
        t->a = a;
        t->b = b;
        t->x = x_local;
        t->x_new = x_new;
        t->n = n;
        t->converged = false;
        t->quit = &quit;
        t->threshold = threshold;

        pthread_create(&t->job, NULL, JacobiRoutine, t);
    }

    /* std::cout << "threads created. GO!" << std::endl; */
    for (int i = 0; i < max_iterations && !stats.solved; i++) {
        pthread_barrier_wait(&barrier); // signal threads
        /* std::cout << "main waiting: i = " << i << std::endl; */
        pthread_barrier_wait(&barrier); // wait for threads
        /* std::cout << "main is checking convergence" << std::endl; */
        swap_buffer((void **) &x_local, (void **) &x_new);

        /* check convergence */
        stats.solved = true;
        for (int th = 0; th < nthreads; th++) {
            if (!threads[th].converged) {
                stats.solved = false;
                break;
            }
        }
        stats.iterations = i;
    }
    quit = true;
    pthread_barrier_wait(&barrier);

    for (int i = 0; i < nthreads; i++)
        pthread_join(threads[i].job, NULL);
    
    pthread_barrier_destroy(&barrier);

    if (stats.solved)
        memcpy(x, x_local, n * sizeof (double));

    delete[] x_new;
    delete[] x_local;

    return stats;
}
