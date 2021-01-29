// program.c

#include "program.h"
#include "plot.h"

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
    if (line[line_len-1] == '\n') // replace trailing newline (\n) with \0
      line[line_len-1] = '\0';
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

void program_plot(program_t *p, char const *file) {
  assert(p);
  assert(file);
  char label[12] = "Z=0.000";
  plot_t *pl = plot_new(file, 400, 400, 10);
  plot_page_init(pl, -100, -100, 600, 600);
  plot_set_rgbaw(pl, 0, 0, 0, 0.05, 2);
  plot_range(pl, -10, -10, 510, 510);
  block_t *b = p->first;
  plot_set_rgbaw(pl, 0.7, 0, 0, 0.8, 2);
  plot_text_at(pl, label, 0, 0);
  plot_begin_poly(pl, 0, 0);
  do {
    if (b->prev && b->prev->target.z != b->target.z) { // change of Z: new page
      plot_end_poly(pl);
      plot_close_page(pl);
      plot_page_init(pl, -100, -100, 600, 600);
      plot_set_rgbaw(pl, 0, 0, 0, 0.05, 2);
      plot_range(pl, -10, -10, 510, 510);
      plot_set_rgbaw(pl, 0, 0, 0, 0.1, 1);
      plot_grid(pl, 0, 0, 500, 500, 50);
      plot_set_rgbas(pl, 0.7, 0, 0, 0.8, 2);
      sprintf(label, "Z=%9.3f", b->target.z);
      plot_text_at(pl, label, 0, 0);
      plot_begin_poly(pl, b->prev->target.x, b->prev->target.y);
    }
    if (b->type == LINE || b->type == RAPID)
      plot_segment_to(pl, b->target.x, b->target.y);
    else if (b->type == ARC_CW) {
      if (b->radius == 0)
        plot_arc_tij_cw(pl, b->target.x, b->target.y, b->center.x, b->center.y);
      else
        plot_arc_tr_cw(pl, b->target.x, b->target.y, b->radius);
    }
    else if (b->type == ARC_CCW) {
      if (b->radius == 0)
        plot_arc_tij_ccw(pl, b->target.x, b->target.y, b->center.x, b->center.y);
      else
        plot_arc_tr_ccw(pl, b->target.x, b->target.y, b->radius);
    }
    b = b->next;
  } while (b);
  plot_end_poly(pl);
  plot_close_page(pl);
  plot_close(pl);
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