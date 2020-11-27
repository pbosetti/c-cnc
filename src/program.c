#include "program.h"

program_t *program_new(char * filename) {
  assert(filename != NULL);
  assert(strlen(filename) > 0);
  program_t *p = malloc(sizeof(program_t));
  assert(p != NULL);
  asprintf(&p->filename, "%s", filename);
  p->first =NULL;
  p->last = NULL;
  p->current = NULL;
  p->n = 0;
  return p;
}

// parse the program
// result is either EXIT_FAILURE or EXIT_SUCCESS
int program_parse(program_t *p, struct machine_config *cfg) {
  assert(p != NULL);
  char *line = NULL;
  ssize_t line_len = 0;
  size_t n = 0;
  block_t *b;

  p->file = fopen(p->filename, "r");
  if (p->file == NULL) {
    fprintf(stderr, "ERROR: cannot open the file %s\n", p->filename);
    return EXIT_FAILURE;
  }

  p->n = 0;
  while ((line_len = getline(&line, &n, p->file)) > 0) {
    line[line_len - 1] = '\0';  // remove trailing newline (\n)
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
  program_reset(p);
  return EXIT_SUCCESS;
}


void program_next(program_t *program) {
  if (program->current == NULL) 
    program->current = program->first;
  else
    program->current = program->current->next;
}

void program_reset(program_t *program) {
  program->current = NULL;
}


// deallocate program and its contents
void program_free(program_t *p) {
  assert(p != NULL);
  block_t *b, *tmp;
  free(p->filename);
  // free blocks, if any
  if (p->n > 0) {
    b = p->first;
    do {
      tmp = b;
      b = b->next;
      block_free(tmp);
    } while (b);
  }
  // free program
  free(p);
}

// print a description of all blocks in program
void program_print(program_t *p, FILE *out) {
  assert(p != NULL);
  block_t *b = p->first;
  do {
    block_print(b, out);
    b = b->next;
  } while(b);
}


#ifdef PROGRAM_MAIN
int main(int argc, const char **argv) {
  int rv;
  program_t *prog;
  struct machine_config cfg = {.A = 10, .D = 5, .tq = 0.005};
  if (argc != 2) {
    fprintf(stderr, "Missing g-code filename!\n");
    return EXIT_FAILURE;
  }

  prog = program_new(argv[1]);
  program_parse(prog, &cfg);

  program_print(prog, stdout);

  program_free(prog);

  return EXIT_SUCCESS;
}
#endif