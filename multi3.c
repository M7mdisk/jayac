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


int p, d, maxfe, n, b, k;
double (*f)(double x[]);

typedef struct {
  int start;
  int end;
  int iters;
} params;

double *solutions;
double *all_fits;

// Shared vars, lock plz
double *global_best_sol, *global_worst_sol;
double global_min_fit = INFINITY;
double global_max_fit = 0;

void *jaya_sub(void *point) {
  params *par = (params *)point;
  printf("start:%i end:%i iter:%i\n", par->start, par->end, par->iters);

  double *r1 = malloc(sizeof(double) * d);
  double *r2 = malloc(sizeof(double) * d);
  double *xi = malloc(sizeof(double) * d);
  double *best_sol = malloc(sizeof(double) * d);
  double *worst_sol = malloc(sizeof(double) * d);

  double min_fit = INFINITY, max_fit = 0;
  double fit, x, new_fit, old_fit;
  int min_idx = -1, max_idx = -1;
  int iter, i, j;

  // determining best and worst sol at the start
  for (i = par->start; i < par->end; i++) {
    fit = f(solutions + (i * d));
    all_fits[i] = fit;

    if (fit < min_fit) {
      min_fit = fit;
      min_idx = i;
      best_sol = solutions + (i * d);
    }

    if (fit > max_fit) {
      max_fit = fit;
      max_idx = i;
      worst_sol = solutions + (i * d);
    }
  }

  for (iter = 0; iter < par->iters; iter++) {
    max_fit = 0;
    // update the solutions
    // xi' = xi + r1 * (best-xi) - r2 * (worst -xi)
    for (i = par->start; i < par->end; i++) {
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

        for (j = 0; j < d; j++)
          solutions[i * d + j] = xi[j];
      }

      if (all_fits[i] > max_fit) {
        max_fit = all_fits[i];
        max_idx = i;
      }

    }

    best_sol = solutions + (min_idx * d);
    worst_sol = solutions + (max_idx * d);

    // TODO: if you are doing this add a mutex
    // if (min_fit < global_min_fit) {
    //   global_min_fit = min_fit;
    // }
#if DEBUG
    // assert(f(best_sol) == min_fit);
    // assert(f(worst_sol) == max_fit);
#endif
  }
  global_best_sol = best_sol;
  pthread_exit(NULL);
}

double *jaya() {
  int k = 6;
  int sub_pop_unit = p / k;
  int merge_times = k - 1;
  int merged = 0;

  int iter_per_merge = n / merge_times;

  pthread_t *threads = malloc(sizeof(pthread_t) * k);
  params *pars = malloc(sizeof(params) * k);

  while (merged < k) {
    // iterations per thread will depend on iter_per_merge divided over the current number of current
    // sub populations.
    // to caluclate the current subpopulation count we do k (starting count) - merged (the number of sub that have been merged
    // or rather 'eaten')
    int iter_per_thread = iter_per_merge / (k - merged);

    pars[0].start = 0;
    pars[0].end = merged * sub_pop_unit + sub_pop_unit;
    pars[0].iters = iter_per_thread;


    pthread_create(threads + 0, NULL, jaya_sub, pars);


    for (int i = 1; i < k - merged; i++) {
      pars[i].start = pars[i - 1].end;
      pars[i].end = pars[i].start + sub_pop_unit;
      pars[i].iters = iter_per_thread;

      // this is to get the last element incase the population and thread count are not divisible
      // this solves the problem of missing the last elements due to rounding
      if (i == k - merged - 1) pars[i].end = p;

      pthread_create(threads + i, NULL, jaya_sub, pars + i);
    }



    for (int i = 0; i < k - merged; i++) {
      pthread_join(threads[i], NULL);
    }

    merged++;
    puts("");
  }

  return NULL;
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
    n = maxfe / p;

  solutions = malloc((p * d) * sizeof(double));
  all_fits = malloc(sizeof(double) * p);
  rand_matrix(solutions, p, d, -b, b);

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
  printf("The fit is %f\n", f(global_best_sol));
  puts("");

  return 0;
}
