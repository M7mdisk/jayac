#if DEBUG
#include <assert.h>
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

double sphere(double x[], int d) {
  double res = 0;
  int i;
  for (i = 0; i < d; i++) {
    res += pow(x[i], 2);
  }
  return res;
}

double rosenbrock(double x[], int d) {
  double res = 0;
  int i;
  for (i = 0; i < d - 1; i++) {
    res += 100 * pow(x[i + 1] - pow(x[i], 2), 2) + pow(1 - x[i], 2);
  }
  return res;
}

double rastrigin(double x[], int d) {
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

/*
 * Given a function f and the number of dimensions d.
 * return the optimal solution found x such that -b < x_i < b initially.
 */
double *jaya(double (*f)(double x[], int d), int d, double b, int n, int p) {
  double *best_sol = malloc(d * sizeof(double));
  double *worst_sol = malloc(d * sizeof(double));
  double *r1 = malloc((1 * d) * sizeof(double));
  double *r2 = malloc((1 * d) * sizeof(double));
  double *xi = malloc(sizeof(double) * d);
  double *all_fits = malloc(sizeof(double) * p);
  double min_fit = INFINITY;
  double max_fit = 0;
  double fit, x, new_fit, old_fit;
  int min_idx = -1, max_idx = -1;
  int iter, i, j;

  double *solutions = malloc((p * d) * sizeof(double));
  rand_matrix(solutions, p, d, -b, b);

#if DEBUG
  FILE *fp;
  fp = fopen("fitness_progression.txt", "w");  // open file for writing
#endif

  for (i = 0; i < p; i++) {
    fit = f(&(solutions[i * d]), d);
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
    // printf("Min fit: %f %i\n", min_fit, iter);

    // update the solutions
    // xi' = xi + r1 * (best-xi) - r2 * (worst -xi)
    // p
    for (i = 0; i < p; i++) {
      rand_matrix(r1, 1, d, 0, 1);
      rand_matrix(r2, 1, d, 0, 1);

      // Create the new solution xi
      for (j = 0; j < d; j++) {
        x = solutions[i * d + j];
        xi[j] = x + r1[j] * (best_sol[j] - x) - r2[j] * (worst_sol[j] - x);
      }
      old_fit = all_fits[i];
      new_fit = f(xi, d);

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
  int p = atoi(argv[1]);
  int d = atoi(argv[2]);
  int maxfe = atoi(argv[3]);
  char *func = argv[4];
  int n = maxfe / p;

  // double *sol = jaya(sphere, d, 30, n);
  double *sol;

  double (*func_ptr)(double x[], int d);

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
  sol = jaya(func_ptr, d, 30, n, p);

  printf("Best solution is: \n");
  for (int i = 0; i < d; i++) {
    printf("%f ", sol[i]);
  }
  puts("");
  printf("The fit is %f\n", func_ptr(sol, d));

  return 0;
}
