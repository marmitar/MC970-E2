#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

static unsigned prime_default(const unsigned n);

// Read exponent 'N' from file
static unsigned read_exponent(const char *filename) {
  FILE *input = fopen(filename, "r");
  if (input == NULL) {
    fprintf(stderr, "Error: could not open file\n");
    return UINT_MAX;
  }

  unsigned exponent = UINT_MAX;
  int rv = fscanf(input, "%u", &exponent);
  fclose(input);

  if (rv != 1) {
    fprintf(stderr, "Error: inputs could not be read correctly\n");
    return UINT_MAX;
  }
  return exponent;
}

int main(const int argc, const char *const restrict argv[argc]) {
  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file\n");
    return EXIT_FAILURE;
  }

  const unsigned N = read_exponent(argv[1]);
  if (N == UINT_MAX) {
    return EXIT_FAILURE;
  }

  const unsigned n_lo = 1;
  const unsigned n_factor = 2;
  const unsigned n_hi = 1 << N;

  // Do not change this line
  omp_set_num_threads(4);

  printf("                    \n");
  printf("         N     Pi(N)\n");
  printf("\n");

  double t = omp_get_wtime();

  for (unsigned n = n_lo; n <= n_hi; n *= n_factor) {
    unsigned primes = prime_default(n);
    printf("  %8u  %8u\n", n, primes);
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

#pragma omp parallel for firstprivate(n) reduction(+ : total) schedule(dynamic)
  for (unsigned i = 2; i <= n; i++) {
    unsigned prime = 1;

    for (unsigned j = 2; j < i; j++) {
      if (i % j == 0) {
        prime = 0;
        break;
      }
    }
    total += prime;
  }

  return total;
}
