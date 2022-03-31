#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

typedef enum {
  ERROR_SUCCESS=0,
  ERROR_ARGS,
  ERROR_READ,
  ERROR_WRITE,
  ERROR_OPEN
} errors_t;

int main(int argc, const char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "I need 2 arguments!\n");
    return ERROR_ARGS;
  }

  const char *filename = argv[1];
  fprintf(stdout, "My filaneme is: %s\n", filename);

  {
    FILE *f = fopen(filename, "w");
    if (!f) {
      fprintf(stderr, "I cannot open the file: %s\n", filename);
      return ERROR_OPEN;
    }

    fprintf(f, "I'm a line\n");
    fprintf(f, "I'm another line\n");
    fprintf(f, "I'm a number: %8.3f\n", 20.0f);

    fclose(f);
  }

  {
    FILE *f = fopen(filename, "r");
    char buffer[MAX_LINE_LENGTH];

    while(fgets(buffer, MAX_LINE_LENGTH, f)) {
      fprintf(stdout, "%s", buffer);
    }
  }

  fprintf(stderr, "I'm an error!");

  return ERROR_SUCCESS;

}