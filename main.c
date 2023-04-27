#if DEBUG
#include <assert.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

// Global because they're heavily shared and don't change throughout run, so its
// a good idea
int p, d, maxfe, n, b;

/*
 * Given a function f and the number of dimensions d.
 * return the optimal solution found x such that -b < x_i < b initially.
 */
double *jaya(double (*f)(double x[])) {
  double *best_sol = malloc(d * sizeof(double));
  double *worst_sol = malloc(d * sizeof(double));
  double *r1 = malloc((1 * d) * sizeof(double));
  double *r2 = malloc((1 * d) * sizeof(double));
  double *xi = malloc(sizeof(double) * d);
  double *all_fits = malloc(sizeof(double) * p);
  double fit, x, new_fit, old_fit;
  double min_fit = INFINITY;
  double max_fit = 0;
  int min_idx = -1, max_idx = -1;
  int iter, i, j;

  double *solutions = malloc((p * d) * sizeof(double));
  rand_matrix(solutions, p, d, -b, b);

#if DEBUG
  FILE *fp;
  fp = fopen("fitness_progression.txt", "w");  // open file for writing

#endif

  for (i = 0; i < p; i++) {
    fit = f(&(solutions[i * d]));
    all_fits[i] = fit;

    if (fit > max_fit) {
      max_fit = fit;
      max_idx = i;
      worst_sol = &(solutions[i * d]);
    }

    if (fit < min_fit) {
      min_fit = fit;
      min_idx = i;
      best_sol = &(solutions[i * d]);
    }
  }

#if DEBUG
  assert(max_idx > 0);
  assert(min_idx > 0);
  fprintf(fp, "%f\n", min_fit);
#endif

  for (iter = 0; iter < n - 1; iter++) {
    // for each solution, find fit and the best/worst

    // update the solutions
    // xi' = xi + r1 * (best-xi) - r2 * (worst -xi)
    for (i = 0; i < p; i++) {
      rand_matrix(r1, 1, d, 0, 1);
      rand_matrix(r2, 1, d, 0, 1);

      // Create the new solution xi
      for (j = 0; j < d; j++) {
        x = solutions[i * d + j];
        xi[j] = x + r1[j] * (best_sol[j] - x) - r2[j] * (worst_sol[j] - x);
      }
      old_fit = all_fits[i];
      new_fit = f(xi);

      // If new one is better, use it instead
      if (new_fit < old_fit) {
        if (new_fit < min_fit) {
          min_fit = new_fit;
          min_idx = i;
        }
        if (new_fit > max_fit) {
          max_fit = new_fit;
          max_idx = i;
        }
        all_fits[i] = new_fit;
        for (j = 0; j < d; j++) solutions[i * d + j] = xi[j];
      }
    }
    best_sol = &(solutions[min_idx * d]);
    worst_sol = &(solutions[max_idx * d]);
#if DEBUG
    fprintf(fp, "%.10f\n", min_fit);
#endif
  }

  min_fit = INFINITY;
  for (int i = 0; i < p; i++) {
    // double *ptr = &solutions[i * d];
    // printf("Fit of %f, %f is %f\n", *ptr, *(ptr + 1), fit);

    if (all_fits[i] < min_fit) {
      min_fit = all_fits[i];
      best_sol = &(solutions[i * d]);
    }
  }

#if DEBUG
  fclose(fp);
#endif

  return best_sol;
}

int main(int argc, char **argv) {
  srand(time(0));

  if (argc != 5) {
    puts("Please enter 4 arguments to this program");
    puts("population size");
    puts("dimension size");
    puts("max FE count");
    puts("loss function name ('sphere', 'rosenbrock', or 'rastrigin')");
    puts("respectively");
    exit(1);
  }
  p = atoi(argv[1]);
  d = atoi(argv[2]);
  maxfe = atoi(argv[3]);
  n = maxfe / p;
  b = 30;
  char *func = argv[4];

  // double *sol = jaya(sphere, d, 30, n);
  double *sol;

  double (*func_ptr)(double x[]);

  if (strcmp(func, "sphere") == 0) {
    func_ptr = sphere;
  } else if (strcmp(func, "rosenbrock") == 0) {
    func_ptr = rosenbrock;
  } else if (strcmp(func, "rastrigin") == 0) {
    func_ptr = rastrigin;
  } else {
    fprintf(stderr, "Invalid function\n");
    exit(1);
  }

  sol = jaya(func_ptr);

  printf("Best solution is: \n");
  for (int i = 0; i < d; i++) {
    printf("%f ", sol[i]);
  }
  puts("");
  printf("The fit is %f\n", func_ptr(sol));

  return 0;
}