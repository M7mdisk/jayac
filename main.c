#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POPULATION_SIZE 10

double sphere(double x[], int d)
{
  double res = 0;
  for (int i = 0; i < d; i++)
  {
    res += pow(x[i], 2);
  }
  return res;
}

double rand_range(double min, double max)
{
  double range = max - min;
  double rand_num = (double)rand() / RAND_MAX;
  double rand_actual = rand_num * range + min;

  return rand_num * range + min;
}

double *initialize_random_solutions(int p, int d, double b)
{
  // Make sure we get new random numbers
  srand(time(0));

  double *res = malloc((p * d) * sizeof(double));
  for (int i = 0; i < p * d; i++)
  {
    res[i] = rand_range(-b, b);
  }
  return res;
}

/*
 * Given a function f and the number of dimensions d.
 * return the optimal solution found x such that -b < x_i < b
 */
double *jaya(int d, double (*f)(double x[], int d), double b)
{
  int p = POPULATION_SIZE;

  double *solutions = initialize_random_solutions(p, d, b);

  double min_fit = INFINITY;
  double *best_sol = malloc(d * sizeof(double));
  // for each solution
  for (int i = 0; i < p; i++)
  {
    double *ptr = &solutions[i * d];
    double fit = f(&(solutions[i * d]), d);
    printf("Fit of %f, %f is %f\n", *ptr, *(ptr + 1), fit);
    if (fit < min_fit)
    {
      min_fit = fit;
      best_sol = &(solutions[i * d]);
    }
  }

  double *ptr = solutions;

  for (int i = 0; i < POPULATION_SIZE; i++)
  {
    for (int j = 0; j < d; j++)
      printf("%f ", ptr[i * d + j]);
    printf("\n");
  }

  return best_sol;
}

int main()
{
  int n = 10;
  // double res = sphere(ar, 2);
  // printf("Test %f \n", res);
  int d = 2;
  double *sol = jaya(d, sphere, 30);

  printf("Best solution is: \n");
  for (int i = 0; i < d; i++)
  {
    printf("%f ", sol[i]);
  }
  puts("");
  printf("The fit is %f\n", sphere(sol, d));

  return 0;
}
