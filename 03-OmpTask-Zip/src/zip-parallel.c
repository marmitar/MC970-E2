#define _GNU_SOURCE
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define _STR(X) #X
#define STR(X) _STR(X)

#define INPUT_STR_SIZE 99

// Read inputs from file
static bool read_input(const char *restrict inputfile,
                       int nthreads[static restrict 1],
                       char strbuf[static restrict INPUT_STR_SIZE + 1]) {

  FILE *input = fopen(inputfile, "r");
  if (input == NULL) {
    fprintf(stderr, "Error: could not open file\n");
    return false;
  }

  // Read inputs
  int sc_nthreads = fscanf(input, "%d", nthreads);
  int sc_string = fscanf(input, "%" STR(INPUT_STR_SIZE) "s", strbuf);
  fclose(input);

  if (sc_nthreads != 1 && sc_string != 1) {
    fprintf(stderr, "Error: inputs could not be read correctly\n");
    return false;
  }
  return true;
}

// Test if password unlocks zip file
static bool password_ok(const char *const filename, const unsigned password) {
  const size_t COMMAND_LENGTH = 400;

  char cmd[COMMAND_LENGTH];
  snprintf(cmd, COMMAND_LENGTH, "unzip -P%u -t %s 2>&1", password, filename);

  FILE *fp = popen(cmd, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: invalid unzip command\n");
    return false;
  }

  while (!feof(fp)) {
    const int LINE_BUF_SIZE = 200;
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

  int nt = -1;
  char filename[INPUT_STR_SIZE + 1] = "";
  bool ok = read_input(argv[1], &nt, filename);
  if (!ok) {
    return EXIT_FAILURE;
  }

  // Do not touch this line
  omp_set_num_threads(nt);

  double t = omp_get_wtime();

// Parallelize this loop using tasks!
#pragma omp parallel default(none) firstprivate(filename, max_password_value)
#pragma omp single nowait
  {
    bool found = false;
    for (unsigned i = 0; i < max_password_value; i++) {
#pragma omp task default(none) firstprivate(filename, i)                       \
    shared(found) if (!found)
      {
        if (!found && password_ok(filename, i)) {
          found = true;
          printf("Password: %u\n", i);
        }
      } // end omp task
    }
  } // end omp parallel

  t = omp_get_wtime() - t;
  fprintf(stderr, "%lf\n", t);

  return EXIT_SUCCESS;
}
