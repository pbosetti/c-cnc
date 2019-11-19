/* File writing/reading examples */
/* Nov. 18th 2019                */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024

typedef unsigned int index_t;
typedef double data_t;

int main(int argc, char const *argv[]) {
  index_t n;
  char const *filename;
  // check for proper command line arguments
  if (argc != 3) {
    fprintf(stderr, "I need file name and a number\n");
    return 1;
  }
  filename = argv[1]; // file arg
  n = atoi(argv[2]);  // numeric arg (integer)
  
  // the following three steps are enclosed in {}
  // this means that they are in separate scopes, i.e.
  // variables declared in each block are not visible elsewhere

  // file write
  { // separate scope: variables defined herein are not visible outside
    index_t i;
    // open filename for reading
    FILE *f = fopen(filename, "w");
    // check if fopen() worked: if so, f is not NULL
    if (f == NULL) {
      fprintf(stderr, "Could not open %s for writing\n", filename);
      return 2;
    }
    // print a header line
    fprintf(f, "#n sq(n)\n");
    // print one line at a time
    for (i = 0; i < n; i++) {
      fprintf(f, "%d; %f\n", i, (float)i*i);
    }
    // close the file (and sync its content with memory)
    fclose(f);
  }

  // READING FROM A FILE
  {
    char line[MAX_LINE_LEN];
    // open filename for reading
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
      fprintf(stderr, "Could not open %s for reading\n", filename);
      return 2;
    }
    // read one line at a time, for a maximum length of 1024 bytes
    // fgets returns NULL when the end of file is reached
    while (fgets(line, MAX_LINE_LEN, f)) {
      printf("%s", line);
    }
    fclose(f);
  }

  // READ FROM FILE AND "INTERPRETATE" THE VALUES
  {
    char line[MAX_LINE_LEN];
    char *word, *str;
    // open filename for reading
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
      fprintf(stderr, "Could not open %s for reading\n", filename);
      return 2;
    }
    while (fgets(line, MAX_LINE_LEN, f)) { // loop over lines in f
      str = line;
      if (str[0] == '#') { // str is a comment starting with '#'
        printf("%s", str);
        continue; // skip next loop
      }
      while ((word = strsep(&str, ",; ")) != NULL) {
        if (strlen(word) == 0) // skip empty words
          continue;
        printf("%f ", atof(word));
      }
      printf("\n");
    }
    fclose(f);
  }
  
  return 0;
}


/*
char line[100] = "this is a list of words";
char *str = line;

word = strsep(&str, " "); // word is "this"

this\0is a list of words\0
^     ^
|     str token
word points here
line points here

word = strsep(&str, " "); // word is "is"

this\0is\0a list of words\0
^     ^   ^
|     |   str token
|     word points here
line still points here

word = strsep(&str, " "); // word is "a"

this\0is\0a\0list of words\0
^         ^  ^
|         |  str token
|         word points here
line still points here

*/
