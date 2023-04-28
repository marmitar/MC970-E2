#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

static bool *prime_sieve(const unsigned max) {
  bool *prime = malloc((max + 1) * sizeof(bool));
  assert(prime != NULL);

  for (unsigned i = 0; i <= max; i++) {
    prime[i] = true;
  }

  prime[0] = prime[1] = false;
  for (unsigned i = 2; i <= max; i++) {
    for (unsigned j = 2 * i; j <= max; j += i) {
      prime[j] = false;
    }
  }
  return prime;
}

static unsigned prime_default(const unsigned n, const bool prime[const n]) {
  unsigned total = 0;
  for (unsigned i = 0; i <= n; i++) {
    if (prime[i]) {
      total += 1;
    }
  }
  return total;
}

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

  bool *prime = prime_sieve(n_hi);
  for (unsigned n = n_lo; n <= n_hi; n *= n_factor) {
    unsigned primes = prime_default(n, prime);
    printf("  %8u  %8u\n", n, primes);
  }

  t = omp_get_wtime() - t;

  /*
    Terminate.
  */
  fprintf(stderr, "%lf\n", t);
  free(prime);

  return EXIT_SUCCESS;
}
