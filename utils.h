#ifndef HELPER
#define HELPER

extern int p, d, maxfe, n, b;

double sphere(double x[]);
double rosenbrock(double x[]);
double rastrigin(double x[]);
double rand_range(double min, double max);
double *rand_matrix(double *res, int n, int m, double min, double max);
void print_matrix(double *mat, int n, int m);

#endif