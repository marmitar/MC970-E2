#define _GNU_SOURCE
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define COMMAND_LENGTH 400ULL
#define LINE_BUF_SIZE 200ULL

// Test if password unlocks zip file
static bool password_ok(const char *const filename, const unsigned password) {
  char cmd[COMMAND_LENGTH];
  snprintf(cmd, COMMAND_LENGTH, "unzip -P%u -t %s 2>&1", password, filename);

  FILE *fp = popen(cmd, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: invalid unzip command\n");
    return false;
  }

  while (!feof(fp)) {
    char ret[LINE_BUF_SIZE];
    if (fgets(ret, LINE_BUF_SIZE, fp) == NULL) {
      break;
    }

    if (strcasestr(ret, "ok") != NULL) {
      pclose(fp);
      return true;
    }
  }

  pclose(fp);
  return false;
}

int main(const int argc, const char *const restrict argv[argc]) {
  const unsigned max_password_value = 500000;

  if (argc < 2) {
    fprintf(stderr, "Error: missing path to input file\n");
    return EXIT_FAILURE;
  }

  FILE *input = fopen(argv[1], "r");
  if (input == NULL) {
    fprintf(stderr, "Error: could not open file\n");
    return EXIT_FAILURE;
  }

  // Read inputs
  int nt;
  fscanf(input, "%d", &nt);
  char filename[100];
  fscanf(input, "%s", filename);

  // Do not touch this line
  omp_set_num_threads(nt);

  double t = omp_get_wtime();

  // Parallelize this loop using tasks!
  for (unsigned i = 0; i < max_password_value; i++) {
    if (password_ok(filename, i)) {
      printf("Password: %u\n", i);
      break;
    }
  }

  t = omp_get_wtime() - t;
  fprintf(stderr, "%lf\n", t);

  return EXIT_SUCCESS;
}
