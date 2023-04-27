#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

static unsigned prime_default(const unsigned n);

int main(const int argc, const char *const restrict argv[argc]) {
  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file\n");
    return EXIT_FAILURE;
  }

  FILE *input = fopen(argv[1], "r");
  if (input == NULL) {
    fprintf(stderr, "Error: could not open file\n");
    return EXIT_FAILURE;
  }

  const unsigned n_lo = 1;
  const unsigned n_factor = 2;

  // Do not change this line
  omp_set_num_threads(4);

  unsigned n_hi;
  fscanf(input, "%u", &n_hi);
  n_hi = 1 << n_hi;

  printf("                    \n");
  printf("         N     Pi(N)\n");
  printf("\n");

  unsigned n = n_lo;

  double t = omp_get_wtime();

  while (n <= n_hi) {
    unsigned primes = prime_default(n);

    printf("  %8u  %8u\n", n, primes);

    n = n * n_factor;
  }

  t = omp_get_wtime() - t;

  /*
    Terminate.
  */
  fprintf(stderr, "%lf\n", t);

  return EXIT_SUCCESS;
}

/*
  Purpose:
   counts primes.
  Licensing:
    This code is distributed under the GNU LGPL license.
  Modified:
    10 July 2010
  Author:
    John Burkardt
  Parameters:
    Input, the maximum number to check.
    Output, the number of prime numbers up to N.
*/
static unsigned prime_default(const unsigned n) {
  unsigned total = 0;

  for (unsigned i = 2; i <= n; i++) {
    unsigned prime = 1;

    for (unsigned j = 2; j < i; j++) {
      if (i % j == 0) {
        prime = 0;
        break;
      }
    }
    total = total + prime;
  }

  return total;
}
