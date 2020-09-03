#include <ctype.h>
#include "block.h"

// static functions are not declared in the header and are 
// only visible from within this file
static data_t quantize(data_t t, data_t tq, data_t *dq) {
  data_t q;
  q = ((index_t)(t / tq) + 1) * tq;
  *dq = q - t;
  return q;
}

static point_t point_zero(block_t *b) {
  point_t p0;
  if (b->prev == NULL) { // this is the first block
    p0 = point_new();
    point_xyz(&p0, b->config->zero[0], b->config->zero[1], b->config->zero[2]);
  } else { // this is NOT the first block
    p0 = b->prev->target;
  }
  return p0;
}

static int block_set_field(block_t *b, char cmd, char *arg) {
  assert(b != NULL);
  switch (cmd) {
    case 'N':
      b->n = atol(arg);
      break;
    case 'G':
      b->type = atoi(arg);
      break;
    case 'X':
      point_x(&b->target, atof(arg));
      break;
    case 'Y':
      point_y(&b->target, atof(arg));
      break;
    case 'Z':
      point_z(&b->target, atof(arg));
      break;
    case 'F':
      b->feedrate = atof(arg);
      break;
    case 'S':
      b->spindle = atof(arg);
      break;
    case 'T':
      b->tool = atoi(arg);
      break;
    case '\0': // empty line
      break;
    default:
      fprintf(stderr, "ERROR: Unexpected G-code command %c%s\n", cmd, arg);
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static void block_compute(block_t *b) {
  assert(b != NULL);
  data_t A, D, a, d;
  data_t dt, dt_1, dt_m, dt_2, dq;
  data_t f_m, l;
  point_t p0 = point_zero(b);

  point_modal(&p0, &b->target);
  point_delta(&p0, &b->target, &b->delta);
  b->length = point_dist(&p0, &b->target);

  A = b->config->A;
  D = b->config->D;
  f_m = b->feedrate / 60.0;
  l = b->length;
  dt_1 = f_m / A;
  dt_2 = f_m / D;
  dt_m = l / f_m - (dt_1 + dt_2) / 2.0;
  if (dt_m > 0) { // trapezoidal profile
    dt = quantize(dt_1 + dt_2 + dt_m, b->config->tq, &dq);
    dt_m += dq;
    f_m = (2 * l) / (dt_1 + dt_2 + 2 * dt_m);
  } else { // triangular profile
    dt_1 = sqrt(2 * l / (A + pow(A, 2) / D));
    dt_2 = dt_1 * A / D;
    dt = quantize(dt_1 + dt_2, b->config->tq, &dq);
    dt_m = 0;
    dt_2 += dq;
    f_m = 2 * l / (dt_1 + dt_2);
  }
  a = f_m / dt_1;
  d = -(f_m / dt_2);

  b->prof->dt_1 = dt_1;
  b->prof->dt_2 = dt_2;
  b->prof->dt_m = dt_m;
  b->prof->a = a;
  b->prof->d = d;
  b->prof->f = f_m;
  b->prof->dt = b->type == RAPID ? 600 : dt;
  b->prof->l = l;
}

// END OF STATIC FUNCTIONS



block_t *block_new(char *line, block_t *prev) {
  // line cannot be NULL, prev can
  assert(line != NULL);
  block_t *b = malloc(sizeof(block_t));
  assert(b != NULL);

  if (prev != NULL) { // there is a former block
    // copy all data from the previous block
    memcpy(b, prev, sizeof(block_t));
    // set prev/next references
    b->prev = prev;
    prev->next = b;
  } else { // this is the first block
    memset(b, 0, sizeof(block_t)); // set all the contents of block to 0
    b->prev = NULL;
  }

  // fields that must be calculated, not inherited from prev
  b->length = 0.0;
  b->target = point_new();
  b->delta = point_new();
  // allocate memory for referenced valus
  b->prof = malloc(sizeof(block_profile_t));
  assert(b->prof != NULL);
  // copy line into b->line
  // asprintf(&b->line, "%s", line);
  b->line = malloc(strlen(line) + 1);
  strcpy(b->line, line);
  return b;
}

void block_free(block_t *block) {
  assert(block != NULL);
  // free allocated fields, if they are not null
  if (block->line != NULL) free(block->line);
  if (block->prof != NULL) free(block->prof);
  // free the block itself
  free(block);
}

int block_parse(block_t *block) {
  assert(block != NULL);
  char *line, *word, *tofree;

  // strdup allocates a new string and does the copy
  // so we need to keep track of the original memory
  // pointer and free it later
  tofree = line = strdup(block->line);
  assert(line != NULL);

  while ((word = strsep(&line, " ")) != NULL) {
    if (block_set_field(block, toupper(word[0]), word + 1) == EXIT_FAILURE) {
      return EXIT_FAILURE;
    }
  }
  free(tofree);
  block_compute(block);
  return EXIT_SUCCESS;
}

data_t block_lambda(block_t *b, data_t t) {
  assert(b != NULL);
  data_t dt_1 = b->prof->dt_1;
  data_t dt_2 = b->prof->dt_2;
  data_t dt_m = b->prof->dt_m;
  data_t a = b->prof->a;
  data_t d = b->prof->d;
  data_t f = b->prof->f;
  data_t r;

  if (t < 0) {
    r = 0.0;
  } else if (t < dt_1) {
    r = a * pow(t, 2) / 2.0;
  } else if (t < (dt_1 + dt_m)) {
    r = f * (dt_1 / 2.0 + (t - dt_1));
  } else if (t < (dt_1 + dt_m + dt_2)) {
    data_t t_2 = dt_1 + dt_m;
    r = f * dt_1 / 2.0 + f * (dt_m + t - t_2) +
        d / 2.0 * (pow(t, 2) + pow(t_2, 2)) - d * t * t_2;
  } else {
    r = b->prof->l;
  }
  return r / b->prof->l;
}

point_t block_interpolate(block_t *b, data_t lambda) {
  assert(b != NULL);
  point_t result = point_new();
  point_t p0 = point_zero(b);

  point_x(&result, p0.x + b->delta.x * lambda);
  point_y(&result, p0.y + b->delta.y * lambda);
  point_z(&result, p0.z + b->delta.z * lambda);
  return result;
}

void block_print(block_t *b, FILE *out) {
  assert(b != NULL);
  point_t p0 = point_zero(b);
  char *t, *p;

  point_inspect(&b->target, &t);
  point_inspect(&p0, &p);

  fprintf(out, "%03d: %s -> %s F%7.1f S%7.1f T%2d (%d)\n", b->n, p, t, b->feedrate, b->spindle, b->tool, b->type);

  // remember to free allocated memory!!!
  free(t);
  free(p);
}
