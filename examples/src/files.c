#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024

typedef double data_t;

typedef enum {
  ERROR_SUCCESS = 0,
  ERROR_ARGS,
  ERROR_WRITE,
  ERROR_READ
} error_t;


int main(int argc, char **argv) {
  size_t n;
  char *filename;

  // we expect to have a filee name and a number
  // on the command line, so we stop if we 
  // don't get exactly three arguments
  if (argc != 3) {
    fprintf(stderr, "I need a file name and a number!\n");
    return ERROR_ARGS;
  }
  filename = argv[1];
  n = atoi(argv[2]); // atof() for reading a float

  // first step: create a file with a table of powers
  {
    size_t i;
    FILE *f = fopen(filename, "w");
    if (!f) {
      fprintf(stderr, "Could not open %s for writing!\n", filename);
      return ERROR_WRITE;
    }
    fprintf(f, "# n sq(n)\n");
    for (i = 0; i < n; i++) {
      fprintf(f, "%ld; %f\n", i, (float)i*i);
    }
    // when done with the file, close it!
    fclose(f);
  }

  // Read back the content from the file and print lines on the console
  {
    FILE *f = fopen(filename, "r");
    char line[MAX_LINE_LEN];
    if (!f) {
      fprintf(stderr, "Could not open %s for reading!\n", filename);
      return ERROR_READ;
    }
    // read the file one line at a time
    while (fgets(line, MAX_LINE_LEN, f)) {
      printf("%s", line);
    }
    fclose(f);
  }

  // now we want to INTERPRETATE the content of the file that we read
  {
    FILE *f = fopen(filename, "r");
    char line[MAX_LINE_LEN];
    char *word, *str;
    if (!f) {
      fprintf(stderr, "Could not open %s for reading!\n", filename);
      return ERROR_READ;
    }

    while (fgets(line, MAX_LINE_LEN, f)) {
      if (line[0] == '#') continue;
      str = line;
      while ((word = strsep(&str, ",; ")) != NULL) {
        if (strlen(word) == 0) continue;
        printf("%f ", atof(word));
      }
      printf("\n");
    }

    fclose(f);
  }

  return ERROR_SUCCESS;
}


/*
char line[100] = "this is a list of words";
char *str = line;

word = strsep(&str, " ");

this\0is a list of words
^     ^
|     str token
word points here
line points here

word = strsep(&str, " ");

this\0is\0a list of words
^     ^   ^
|     |   str points here
|     word points here
line points here

word = strsep(&str, " ");

this\0is\0a\0list of words
^         ^  ^
|         |  str points here
|         word points here
line points here

*/