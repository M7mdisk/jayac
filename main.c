#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEBUG 0

#define POPULATION_SIZE 100

double sphere(double x[], int d) {
  double res = 0;
  for (int i = 0; i < d; i++) {
    res += pow(x[i], 2);
  }
  return res;
}

double rosenbrock(double x[], int d) {
  double res = 0;
  for (int i = 0; i < d - 1; i++) {
    res += 100 * pow(x[i + 1] - pow(x[i], 2), 2) + pow(1 - x[i], 2);
  }
  return res;
}

double rastrigin(double x[], int d) {
  double res = 0;
  for (int i = 0; i < d; i++) {
    res += pow(x[i], 2) - 10 * cos(2 * M_PI * x[i]) + 10;
  }
  return res;
}

double rand_range(double min, double max) {
  double range = max - min;
  double rand_num = (double)rand() / RAND_MAX;
  double rand_actual = rand_num * range + min;

  return rand_num * range + min;
}

double *rand_matrix(int n, int m, double min, double max) {
  // Make sure we get new random numbers
  srand(time(0));

  double *res = malloc((n * m) * sizeof(double));
  for (int i = 0; i < n * m; i++) {
    res[i] = rand_range(min, max);
  }
  return res;
}

void print_matrix(double *mat, int n, int m) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) printf("%f ", mat[i * m + j]);
    printf("\n");
  }
}

/*
 * Given a function f and the number of dimensions d.
 * return the optimal solution found x such that -b < x_i < b initially.
 */
double *jaya(double (*f)(double x[], int d), int d, double b, int n) {
  int p = POPULATION_SIZE;

  double *solutions = rand_matrix(p, d, -b, b);

  double *best_sol = malloc(d * sizeof(double));

  for (int iter = 0; iter < n; iter++) {
    double min_fit = INFINITY;
    best_sol = malloc(d * sizeof(double));

    double max_fit = 0;
    double *worst_sol = malloc(d * sizeof(double));

    // for each solution, find fit and the best/worst
    for (int i = 0; i < p; i++) {
      double fit = f(&(solutions[i * d]), d);

      if (fit > max_fit) {
        max_fit = fit;
        worst_sol = &(solutions[i * d]);
      }

      if (fit < min_fit) {
        min_fit = fit;
        best_sol = &(solutions[i * d]);
      }
    }

    // update the solutions
    if (DEBUG) {
      printf("Before:------------------------\n");
      print_matrix(solutions, p, d);
    }
    // xi' = xi + r1 * (best-xi) - r2 * (worst -xi)
    for (int i = 0; i < p; i++) {
      double *r1 = rand_matrix(1, d, 0, 1);
      double *r2 = rand_matrix(1, d, 0, 1);

      // Create the new solution xi
      double *xi = malloc(sizeof(double) * d);
      for (int j = 0; j < d; j++) {
        double x = solutions[i * d + j];
        xi[j] = x + r1[j] * (best_sol[j] - x) - r2[j] * (worst_sol[j] - x);
      }
      double old_fit = f(&(solutions[i * d]), d);
      double new_fit = f(xi, d);
      // If new one is better, use it instead
      if (new_fit < old_fit) {
        for (int j = 0; j < d; j++) solutions[i * d + j] = xi[j];
      }
    }
    if (DEBUG) {
      printf("After:------------------------\n");
      print_matrix(solutions, p, d);
    }
  }

  if (DEBUG) print_matrix(solutions, p, d);
  double min_fit = INFINITY;
  for (int i = 0; i < p; i++) {
    // double *ptr = &solutions[i * d];
    // printf("Fit of %f, %f is %f\n", *ptr, *(ptr + 1), fit);

    double fit = f(&(solutions[i * d]), d);

    if (fit < min_fit) {
      min_fit = fit;
      best_sol = &(solutions[i * d]);
    }
  }

  return best_sol;
}

// TODO: Add max function evaluations
int main() {
  int n = 1000, d = 2;
  double *sol = jaya(rosenbrock, d, 30, n);

  printf("Best solution is: \n");
  for (int i = 0; i < d; i++) {
    printf("%f ", sol[i]);
  }
  puts("");
  printf("The fit is %f\n", rosenbrock(sol, d));

  return 0;
}
