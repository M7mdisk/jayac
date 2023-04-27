#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double sphere(double x[]) {
  double res = 0;
  int i;
  for (i = 0; i < d; i++) {
    res += pow(x[i], 2);
  }
  return res;
}

double rosenbrock(double x[]) {
  double res = 0;
  int i;
  for (i = 0; i < d - 1; i++) {
    res += 100 * pow(x[i + 1] - pow(x[i], 2), 2) + pow(1 - x[i], 2);
  }
  return res;
}

double rastrigin(double x[]) {
  double res = 0;
  int i;
  for (i = 0; i < d; i++) {
    res += pow(x[i], 2) - 10 * cos(2 * M_PI * x[i]) + 10;
  }
  return res;
}

double rand_range(double min, double max) {
  double range = max - min;
  double rand_num = (double)rand() / RAND_MAX;

  return rand_num * range + min;
}

double *rand_matrix(double *res, int n, int m, double min, double max) {
  // Make sure we get new random numbers

  int i;
  for (i = 0; i < n * m; i++) {
    res[i] = rand_range(min, max);
  }
  return res;
}

void print_matrix(double *mat, int n, int m) {
  int i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) printf("%f ", mat[i * m + j]);
    printf("\n");
  }
}