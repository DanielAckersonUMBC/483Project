#pragma once

struct SolutionStats {
    bool solved;
    int iterations;
    double time_sec;
};

class ParallelJacobiSolver {
    int nthreads;
    int max_iterations;
    double threshold;

public:
    ParallelJacobiSolver(int threads, int max_iterations = 2000, double threshold = 1e-10);

    void setThreads(int n);
    int getThreads();

    void setMaxIterations(int i);
    int getMaxIterations();

    void setSolutionThreshold(double t);
    double getSolutionThreshold();

    SolutionStats solve(int n, double const a[], double const b[], double x[]);
};
