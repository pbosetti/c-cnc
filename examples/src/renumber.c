//   ____                            _               
//  |  _ \ ___ _ __  _   _ _ __ ___ | |__   ___ _ __ 
//  | |_) / _ \ '_ \| | | | '_ ` _ \| '_ \ / _ \ '__|
//  |  _ <  __/ | | | |_| | | | | | | |_) |  __/ |   
//  |_| \_\___|_| |_|\__,_|_| |_| |_|_.__/ \___|_|   
//  Read a Gcode file and normalize its N commands
//  Also, capitalize all commands and remove leading whitespaces
//  Compile as clang src/renumber.c -lm -o renumber
//  This program is designed to work as a filter:
//  cat -e gcode.txt | renumber - | c-cnc -
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <libgen.h>
#include <sys/param.h>

// Initial buffer size, it will increase if lines are longer
#define INI_BUFSIZE 128

int main(int argc, char const *argv[]) {
  FILE *fp;
  int n = 1;
  size_t buf_len = INI_BUFSIZE, newline_len = INI_BUFSIZE;
  size_t needed;
  char *line = NULL, *word = NULL, *to_free = NULL;
  char *buffer = calloc(sizeof(char), buf_len);
  char *newline = calloc(sizeof(char), newline_len);

  // command line parsing
  if (argc == 1) { // brief help
    char *this = strdup(argv[0]);
    printf("Usage: %s [filename | -], where '-' means standard input\n", basename(this));
    return 0;
  }
  else if (argc == 2) {
    // a single dash means "read from stdin"
    if (strcmp(argv[1], "-") == 0) {
      fp = stdin;
    }
    // assume the argument is the name of the input file
    else {
      fp = fopen(argv[1], "r");
      if (!fp) {
        perror("Could not open file");
        return 1;
      }
    }
  }


  // getline reads untile endline or end of file
  // if the buffer is not large enough, it reallocates it and
  // writes the new size in buf_len
  while (getline(&buffer, &buf_len, fp) > -1) {
    // make a copy. WARNING: strdup uses malloc internally!
    to_free = line = strdup(buffer);
    // remove leading spaces
    while (isspace(*line)) line++;
    // skip if empty
    if (strlen(line) <= 1) {
      free(to_free);
      continue;
    }
    // print verbatim if it is a comment line
    if (line[0] == ';') {
      printf("%s", line);
      free(to_free);
      continue;
    }
    // remove new line char
    if (line[strlen(line)-1] == '\n') {
      line[strlen(line)-1] = '\0';
    }
    // if the new_line buffer is too short, make it as long as buffer
    if (buf_len > newline_len) {
      // 'N125 ' takes 1 + int(log10(n)+1) + 1
      newline_len = buf_len + (size_t)log10(n) + 3;
      newline = realloc(newline, newline_len);
    }
    memset(newline, 0, newline_len);
    snprintf(newline, newline_len, "N%d", n++);
    // tokenize line
    while ((word = strsep(&line, " ")) != NULL) {
      if (word[0] == ';') {
        // trailing comment: attach the remaining and skip to next line
        // snprintf(newline, newline_len, "%s %s %s", newline, word, line);
        strncat(newline, " ", newline_len);
        strncat(newline, word, newline_len);
        strncat(newline, line, newline_len);
        break;
      }
      else if (toupper(word[0]) != 'N') {
        word[0] = toupper(word[0]);
        strncat(newline, " ", newline_len);
        strncat(newline, word, newline_len);
        // snprintf(newline, newline_len, "%s %s", newline, word);
      }
    }
    free(to_free);
    // print to stdout
    printf("%s\n", newline);
  }

  // cleanup
  free(buffer);
  free(newline);
  if (fp != stdin) {
    fclose(fp);
  }
  
  return 0;
}
