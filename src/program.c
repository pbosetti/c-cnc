// program.c

#include "program.h"

program_t *program_new(const char *filename) {
  assert(filename);
  assert(strlen(filename) > 0);

  program_t *p = malloc(sizeof(program_t));
  assert(p);
  asprintf(&p->filename, "%s", filename);
  p->first = NULL;
  p->last = NULL;
  p->current = NULL;
  p->n = 0;
  return p;
}

// deallocate
void program_free(program_t *p) {
  assert(p);
  block_t *b, *tmp;
  if (p->n > 0) {
    b = p->first;
    do {
      tmp = b;
      b = b->next;
      block_free(tmp);
    } while (b);
  }
  free(p->filename);
  free(p);
}

// parse the program
// return either EXIT_SUCCESS or EXIT_FAILURE
int program_parse(program_t *p, struct machine_config *cfg) {
  assert(p);
  char *line = NULL;
  ssize_t line_len = 0;
  size_t n = 0;

  block_t *b;

  p->file = fopen(p->filename, "r");
  if (p->file == NULL) {
    fprintf(stderr, "ERROR: canot open the file %s\n", p->filename);
    return EXIT_FAILURE;
  }

  p->n = 0;
  while ( (line_len = getline(&line, &n, p->file)) >= 0) {
    line[line_len-1] = '\0'; // remove trailing newline (\n) replacing it with a terminator
    b = block_new(line, p->last, cfg);
    if (block_parse(b) == EXIT_FAILURE) {
      fprintf(stderr, "ERROR: parsing the block %s\n", line);
      return EXIT_FAILURE;
    }
    if (p->first == NULL) p->first = b;
    p->last = b;
    p->n++;
  }
  fclose(p->file);
  free(line);
  program_reset(p);
  return EXIT_SUCCESS;
}

// linked-list navigation functions
void program_next(program_t *p) {
  assert(p);
  if (p->current == NULL) p->current = p->first;
  else p->current = p->current->next;
}

void program_reset(program_t *p) {
  assert(p);
  p->current = NULL;
}

// print a program description
void program_print(program_t *p, FILE *output) {
  assert(p);
  block_t *b = p->first;
  do {
    block_print(b, output);
    b = b->next;
  } while (b);
}

#ifdef PROGRAM_MAIN
int main(int argc, const char **argv) {
  program_t *prog;
  struct machine_config cfg = {.A = 10, .D = 5, .tq = 0.005};

  if (argc != 2) {
    fprintf(stderr, "ERROR: Missing g-code file in command line!\n");
    return EXIT_FAILURE;
  }

  prog = program_new(argv[1]);

  program_parse(prog, &cfg);
  program_print(prog, stdout);

  program_free(prog);

  return EXIT_SUCCESS;
}

#endif