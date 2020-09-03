#include "program.h"

program_t *program_new(char * filename) {
  assert(filename != NULL);
  assert(strlen(filename) > 0);
  program_t *p = malloc(sizeof(program_t));
  assert(p != NULL);
  asprintf(&p->filename, "%s", filename);
  p->first =NULL;
  p->last = NULL;
  p->n = 0;
  return p;
}

// deallocate program and its contents
void program_free(program_t *p) {
  assert(p != NULL);
  block_t *b, *tmp;
  free(p->filename);
  fclose(p->file);

  b = p->first;
  do {
    tmp = b;
    b = b->next;
    block_free(tmp);
  } while (b);
  free(p);
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
    b = block_new(line, p->last);
    b->config = cfg;
    if (block_parse(b) == EXIT_FAILURE) {
      fprintf(stderr, "ERROR: parsing the block %s\n", line);
      return EXIT_FAILURE;
    }
    if (p->first == NULL) p->first = b;
    p->last = b;
    p->n++;
  }
  return EXIT_SUCCESS;
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

// loop over the whole program
void program_loop(program_t *program,
        timeloop_t timestep,
        blockloop_t new_block,
        void *userdata) {
  assert(program != NULL);
  assert(timestep != NULL);
  data_t t;
  block_t *b = program->first;

  do { // loop on G-code blocks
    if (new_block != NULL) {
      new_block(b, userdata);
    }
    for (t = 0.0; t < b->prof->dt; t += b->config->tq) {
      if(timestep(b, t, userdata) == STOP) {
        break;
      }
    }
    b = b->next;
  } while (b);
}