#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define SEED 123

typedef int *restrict *matrix_t;
typedef const int *const restrict *const const_matrix_t;

static void free_matrix(matrix_t m, unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    free(m[i]);
  }
  free((void *)m);
}

static matrix_t mul(matrix_t restrict a, matrix_t restrict b, unsigned size) {
  matrix_t ret = malloc(size * sizeof(int *));
  for (unsigned i = 0; i < size; i++) {
    ret[i] = calloc(size, sizeof(int));
    for (unsigned j = 0; j < size; j++) {
      for (unsigned k = 0; k < size; k++) {
        ret[i][j] += a[i][k] * b[k][j];
      }
    }
  }

  free_matrix(a, size);
  free_matrix(b, size);

  return ret;
}

// Parallelise this function:
static matrix_t array_mul(unsigned n, matrix_t restrict data[const n],
                          unsigned size) {
  matrix_t ret = data[0];
  for (unsigned i = 1; i < n; i++) {
    ret = mul(ret, data[i], size);
  }
  return ret;
}

static matrix_t rnd_matrix(unsigned size) {
  matrix_t ret = malloc(size * sizeof(int *));
  for (unsigned i = 0; i < size; i++) {
    ret[i] = malloc(size * sizeof(int));
    for (unsigned j = 0; j < size; j++) {
      ret[i][j] = 2 * (rand() % 2) - 1; // Generates -1 or 1
    }
  }

  return ret;
}

static void print_matrix(const_matrix_t m, unsigned size) {
  for (unsigned i = 0; i < size; i++) {
    for (unsigned j = 0; j < size; j++) {
      printf("%d ", m[i][j]);
    }
    printf("\n");
  }
}

int main(const int argc, const char *const restrict argv[const argc]) {
  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file!\n");
    return EXIT_FAILURE;
  }

  FILE *input = fopen(argv[1], "r");
  if (input == NULL) {
    fprintf(stderr, "Error: could not open input file!\n");
    return EXIT_FAILURE;
  }

  unsigned n = 0, size = 0;
  fscanf(input, "%u %u", &n, &size);
  srand(SEED);

  // Do not change this line
  omp_set_num_threads(4);

  matrix_t *data = malloc(n * sizeof(int **));
  for (unsigned i = 0; i < n; i++) {
    data[i] = rnd_matrix(size);
  }

  double t = omp_get_wtime();
  matrix_t ret = array_mul(n, data, size);
  t = omp_get_wtime() - t;

  print_matrix((const_matrix_t)ret, size);
  fprintf(stderr, "%lf\n", t);

  free_matrix(ret, size);
  free(data);
  return EXIT_SUCCESS;
}
