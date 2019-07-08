/*
 * jacobi linear solver with concurrency using pthreads
 *
 * Based on jacobi.cpp from https://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi/jacobi.html
 * Direct link: https://people.sc.fsu.edu/~jburkardt/cpp_src/jacobi/jacobi.cpp
 */

#include "ParallelJacobiSolver.hpp"
#include "jacobi.hpp"

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
    int begin, end;
    int n;
    int id;
    bool volatile *quit;
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

    pthread_barrier_wait(t->barrier); // wait for main thread
    while (! *t->quit) {
        for (int i = t->begin; i < t->end; i++) {
            t->x_new[i] = t->b[i];
            for (int j = 0; j < t->n; j++) {
                if (i != j)
                    t->x_new[i] -= t->a[j + i * t->n] * t->x[j];
            }
            t->x_new[i] /= t->a[i + i * t->n];
        }

        swap_buffer((void **) &t->x, (void **) &t->x_new);

        pthread_barrier_wait(t->barrier); // signal main thread
        pthread_barrier_wait(t->barrier); // wait for main thread
    }

    return NULL;
}


SolutionStats ParallelJacobiSolver::solve(int n, double const a[], double const b[], double x[])
{
    SolutionStats stats;
    SolverThread threads[nthreads];
    pthread_barrier_t barrier;
    double *x_local = new double[n];
    double *x_new = new double[n];
    bool volatile quit = false;
    timespec start, stop;

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
        t->quit = &quit;

        pthread_create(&t->job, NULL, JacobiRoutine, t);
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (   stats.iterations = 0;
            stats.iterations < max_iterations && !stats.solved; 
            stats.iterations++) {

        pthread_barrier_wait(&barrier); // signal threads
        pthread_barrier_wait(&barrier); // wait for threads

        /* check convergence */
        stats.norm = sqrt(r8vec_diff_norm_squared(n, x_new, x_local));
        if (stats.norm < threshold)
            stats.solved = true;

        swap_buffer((void **) &x_local, (void **) &x_new);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    quit = true;
    pthread_barrier_wait(&barrier); // signal threads

    for (int i = 0; i < nthreads; i++)
        pthread_join(threads[i].job, NULL);
    
    pthread_barrier_destroy(&barrier);

    memcpy(x, x_local, n * sizeof (double));
    stats.time_sec = stop.tv_sec - start.tv_sec;
    stats.time_sec += (stop.tv_nsec - start.tv_nsec) * 1e-9;

    delete[] x_new;
    delete[] x_local;

    return stats;
}
