#define _GNU_SOURCE
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

int main(const int argc, const char *const restrict argv[argc]) {
  const int max_password_value = 500000;

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
  for (int i = 0; i < max_password_value; i++) {
    char cmd[400];
    sprintf((char *)&cmd, "unzip -P%d -t %s 2>&1", i, filename);

    FILE *fp = popen(cmd, "r");
    while (!feof(fp)) {
      char ret[200];
      fgets((char *)&ret, 200, fp);
      if (strcasestr(ret, "ok") != NULL) {
        printf("Password: %d\n", i);
        i = 500000;
      }
    }
    pclose(fp);
  }

  t = omp_get_wtime() - t;
  fprintf(stderr, "%lf\n", t);

  return EXIT_SUCCESS;
}
