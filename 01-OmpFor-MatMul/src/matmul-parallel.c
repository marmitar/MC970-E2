
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <omp.h>

// Initialize matrices
static void initialize_matrices(const unsigned size,
                                float a[restrict size * size],
                                float b[restrict size * size],
                                float c[restrict size * size],
                                const unsigned seed) {
  srand(seed);
  for (unsigned i = 0; i < size; ++i) {
    for (unsigned j = 0; j < size; ++j) {
      a[i * size + j] = (float)(rand() % 10);
      b[i * size + j] = (float)(rand() % 10);
      c[i * size + j] = 0.0f;
    }
  }
}

// Parallelize this function using OpenMP
static void multiply(const unsigned size, const float a[restrict size * size],
                     const float b[restrict size * size],
                     float c[restrict size * size]) {
  for (unsigned i = 0; i < size; ++i) {
    for (unsigned j = 0; j < size; ++j) {
      float sum = 0.0;
      for (unsigned k = 0; k < size; ++k) {
        sum = sum + a[i * size + k] * b[k * size + j];
      }
      c[i * size + j] = sum;
    }
  }
}

// Output matrix to stdout
static void print_matrix(const unsigned size, const float c[size * size]) {
  for (unsigned i = 0; i < size; i++) {
    for (unsigned j = 0; j < size; j++) {
      printf(" %5.1f", c[i * size + j]);
    }
    printf("\n");
  }
}

// Read inputs from filename
static bool read_input(const char *restrict filename, unsigned size[static
restrict 1], unsigned seed[static restrict 1]) {
  FILE *input = fopen(filename, "r");
  if (input == NULL) {
    fprintf(stderr, "Error: could not open file\n");
    return false;
  }

  // Read inputs
  int sc_size = fscanf(input, "%u", size);
  int sc_seed = fscanf(input, "%u", seed);
  fclose(input);

  if (sc_size != 1 || sc_seed != 1) {
    fprintf(stderr, "Error: inputs could not be read correctly\n");
    return false;
  }
  return true;
}

int main(const int argc, const char *const restrict argv[argc]) {
  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file\n");
    return EXIT_FAILURE;
  }

  // Read inputs
  unsigned seed = 0, size = 0;
  bool ok = read_input(argv[1], &size, &seed);
  if (!ok) {
    return EXIT_FAILURE;
  }

  // Do not change this line
  omp_set_num_threads(4);

  // Allocate matrices
  float *a = malloc(sizeof(float) * size * size);
  float *b = malloc(sizeof(float) * size * size);
  float *c = malloc(sizeof(float) * size * size);

  // initialize_matrices with random data
  initialize_matrices(size, a, b, c, seed);

  // Multiply matrices
  double t = omp_get_wtime();
  multiply(size, a, b, c);
  t = omp_get_wtime() - t;

  // Show result
  print_matrix(size, c);

  // Output elapsed time
  fprintf(stderr, "%lf\n", t);

  // Release memory
  free(a);
  free(b);
  free(c);

  return EXIT_SUCCESS;
}
