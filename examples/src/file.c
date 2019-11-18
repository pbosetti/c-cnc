#include <stdlib.h>
#include <stdio.h>

typedef unsigned int index_t;
typedef double data_t;

int main(int argc, char const *argv[]) {
  char *filename;
  index_t n;

  if (argc != 3) {
    printf("Error: I need file name and a number!\n");
    return 1;
  }
  filename = (char *)argv[1];
  n = atoi(argv[2]); // atof() for real numbers

  // WRITING TO A FILE
  {
    index_t i;
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
      printf("Error: Cannot open file %s for writing\n", filename);
      return 2;
    }
    fprintf(f, "#n sq(n)\n");
    for (i = 0; i < n; i++) {
      fprintf(f, "%d %f\n", i, (float)i*i);
    }
    fclose(f);
  }

  // READING FROM A FILE
  {
    char line[1024];
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
      printf("Error: Cannot open file %s for reading\n", filename);
      return 3;
    }
    while (fgets(line, 1024, f)) {
      printf("%s\n", line);
    }
    fclose(f);
  }
  
  return 0;
}
