#if DEBUG
#include <assert.h>
#endif
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

// Global because they're heavily shared and don't change throughout run, so its
// a good idea
int p, d, maxfe, n, b, k, subsize;
double (*f)(double x[]);

double *global_best_sol;
double global_best_fit = INFINITY;

// Struct for result from thread
typedef struct solution_t {
  double fit;
  double *sol;

} solution;

void *jaya_sub() {
  int p = subsize;
  double min_fit = INFINITY, max_fit = 0;
  double *r1 = malloc((1 * d) * sizeof(double));
  double *r2 = malloc((1 * d) * sizeof(double));
  double *xi = malloc(sizeof(double) * d);
  double *all_fits = malloc(sizeof(double) * p);
  double fit, x, new_fit, old_fit;
  int min_idx = -1, max_idx = -1;
  int iter, i, j;
  double *best_sol = malloc(sizeof(double) * d);
  double *worst_sol = malloc(sizeof(double) * d);

  double *solutions = malloc((p * d) * sizeof(double));
  rand_matrix(solutions, p, d, -b, b);

  for (i = 0; i < p; i++) {
    fit = f(&(solutions[i * d]));
    all_fits[i] = fit;

    if (fit < min_fit) {
      min_fit = fit;
      min_idx = i;
      best_sol = &(solutions[i * d]);
    }

    if (fit > max_fit) {
      max_fit = fit;
      max_idx = i;
      worst_sol = &(solutions[i * d]);
    }
  }

  for (iter = 0; iter < n - 1; iter++) {
    max_fit = 0;
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
        all_fits[i] = new_fit;
        for (j = 0; j < d; j++) solutions[i * d + j] = xi[j];
      }
      if (all_fits[i] > max_fit) {
        max_fit = all_fits[i];
        max_idx = i;
      }
    }
    best_sol = &(solutions[min_idx * d]);
    worst_sol = &(solutions[max_idx * d]);
#if DEBUG
    // assert(f(best_sol) == min_fit);
    // assert(f(worst_sol) == max_fit);
#endif
  }

  solution *final_sol = malloc(sizeof(solution));
  final_sol->fit = min_fit;
  final_sol->sol = best_sol;

  pthread_exit(final_sol);
}

void jaya() {
  subsize = p / k;

  pthread_t *threads = malloc(sizeof(pthread_t) * k);

  for (int i = 0; i < k; i++) {
    pthread_create(&threads[i], NULL, jaya_sub, NULL);
  }

  solution **results = malloc(k * sizeof(solution));

  for (int i = 0; i < k; i++) pthread_join(threads[i], &results[i]);

  // get the best sol out of the threads
  for (int i = 0; i < k; i++) {
    if (results[i]->fit < global_best_fit) {
      global_best_fit = results[i]->fit;
      global_best_sol = results[i]->sol;
    }
  }
}

int main(int argc, char **argv) {
  srand(time(0));

  if (argc != 6) {
    puts("Please enter 5 arguments to this program");
    puts("Population size (n)");
    puts("Dimensions (d)");
    puts("maxFE");
    puts("loss function name ('sphere', 'rosenbrock', or 'rastrigin')");
    puts("Number of sub-populations (K)");
    puts("respectively");
    exit(1);
  }
  p = atoi(argv[1]);
  d = atoi(argv[2]);
  maxfe = atoi(argv[3]);
  b = 30;
  char *func = argv[4];
  k = atoi(argv[5]);
  n = maxfe / k / p;

  if (strcmp(func, "sphere") == 0) {
    f = sphere;
  } else if (strcmp(func, "rosenbrock") == 0) {
    f = rosenbrock;
  } else if (strcmp(func, "rastrigin") == 0) {
    f = rastrigin;
  } else {
    fprintf(stderr, "Invalid function\n");
    exit(1);
  }

  jaya();

  printf("Best solution is: \n");
  for (int i = 0; i < d; i++) {
    printf("%f ", global_best_sol[i]);
  }
  puts("");
  printf("The fit is %f\n", global_best_fit);

  return 0;
}
