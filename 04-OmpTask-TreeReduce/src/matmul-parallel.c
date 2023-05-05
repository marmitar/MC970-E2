#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

#define SEED 123

typedef int *restrict *matrix_t;
typedef const int *const restrict *const const_matrix_t;

static void free_matrix(matrix_t m, const unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    free(m[i]);
  }
  free((void *)m);
}

static matrix_t cmul(const_matrix_t restrict a, const_matrix_t restrict b,
                     const unsigned size) {
  matrix_t ret = malloc(size * sizeof(int *));
  assert(ret != NULL);

  for (unsigned i = 0; i < size; i++) {
    ret[i] = calloc(size, sizeof(int));
    assert(ret[i] != NULL);
  }

  for (unsigned i = 0; i < size; i++) {
    for (unsigned j = 0; j < size; j++) {
      for (unsigned k = 0; k < size; k++) {
        ret[i][j] += a[i][k] * b[k][j];
      }
    }
  }
  return ret;
}

static matrix_t mul(matrix_t restrict a, matrix_t restrict b,
                    const unsigned size) {
  matrix_t ret = cmul((const_matrix_t)a, (const_matrix_t)b, size);
  free_matrix(a, size);
  free_matrix(b, size);
  return ret;
}

static matrix_t array_mul_rec(const unsigned size, const unsigned n,
                              matrix_t restrict data[const n]) {
  if (n <= 1) {
    assert(n > 0);
    return data[0];
  }

  matrix_t left = NULL, right = NULL;
#pragma omp task default(none) shared(size, n, data, left) depend(out : left)
  left = array_mul_rec(size, n / 2, data);
#pragma omp task default(none) shared(size, n, data, right) depend(out : right)
  right = array_mul_rec(size, n - n / 2, &data[n / 2]);
#pragma omp taskwait

  return mul(left, right, size);
}

// Parallelise this function:
static matrix_t array_mul(const unsigned n, matrix_t restrict data[const n],
                          const unsigned size) {
  matrix_t ret = NULL;
#pragma omp parallel default(none) shared(n, size, data, ret)
#pragma omp single nowait
  ret = array_mul_rec(size, n, data);

  free((void *)data);
  return ret;
}

static matrix_t rnd_matrix(const unsigned size) {
  matrix_t ret = malloc(size * sizeof(int *));
  assert(ret != NULL);

  for (unsigned i = 0; i < size; i++) {
    ret[i] = malloc(size * sizeof(int));
    assert(ret[i] != NULL);
  }

  for (unsigned i = 0; i < size; i++) {
    for (unsigned j = 0; j < size; j++) {
      ret[i][j] = 2 * (rand() % 2) - 1; // Generates -1 or 1
    }
  }

  return ret;
}

static void print_matrix(const_matrix_t m, const unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    for (unsigned j = 0; j < size; j++) {
      printf("%d ", m[i][j]);
    }
    printf("\n");
  }
}

// Read sizes from file
static bool read_input(const char *restrict filename,
                       unsigned n[static restrict 1],
                       unsigned size[static restrict 1]) {

  FILE *input = fopen(filename, "r");
  if (input == NULL) {
    fprintf(stderr, "Error: could not open file\n");
    return false;
  }

  // Read inputs
  int sc_count = fscanf(input, "%u %u", n, size);
  fclose(input);

  if (sc_count != 2) {
    fprintf(stderr, "Error: inputs could not be read correctly\n");
    return false;
  }
  if (*n == 0 || *size == 0) {
    fprintf(stderr, "Error: invalid zero-sized arrays\n");
    return false;
  }
  return true;
}

int main(const int argc, const char *const restrict argv[const argc]) {
  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file!\n");
    return EXIT_FAILURE;
  }

  unsigned n = 0, size = 0;
  bool ok = read_input(argv[1], &n, &size);
  if (!ok) {
    return EXIT_FAILURE;
  }

  srand(SEED);

  // Do not change this line
  omp_set_num_threads(4);

  matrix_t *data = malloc(n * sizeof(int **));
  assert(data != NULL);
  for (unsigned i = 0; i < n; i++) {
    data[i] = rnd_matrix(size);
  }

  double t = omp_get_wtime();
  matrix_t ret = array_mul(n, data, size);
  t = omp_get_wtime() - t;

  print_matrix((const_matrix_t)ret, size);
  fprintf(stderr, "%lf\n", t);

  free_matrix(ret, size);
  return EXIT_SUCCESS;
}
