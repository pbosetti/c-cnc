#include "block.h"
#include <ctype.h>


//   ____       _            _
//  |  _ \ _ __(_)_   ____ _| |_ ___
//  | |_) | '__| \ \ / / _` | __/ _ \
//  |  __/| |  | |\ V / (_| | ||  __/
//  |_|   |_|  |_| \_/ \__,_|\__\___|
// Static, or private, functions
// are only accessible from within this file

// quantize a time interval as a multiple of the sampling time
// tq; put the difference in dq
static data_t quantize(data_t t, data_t tq, data_t *dq) {
  data_t q;
  q = ((index_t)(t / tq) + 1) * tq;
  *dq = q - t;
  return q;
}

// set individual fields for a G-code word, made by a command
// (single letter) and an argument (number as a string)
static int block_set_field(block_t *b, char cmd, char *arg) {
  assert(b);
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
    default:
      fprintf(stderr, "ERROR: Unexpected G-code command %c%s\n", cmd, arg);
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

// compute velocity profile for the block
static void block_compute(block_t *b) {
  assert(b);
  data_t A, D, a, d;
  data_t dt, dt_1, dt_2, dt_m, dq;
  data_t f_m, l;

  A = b->config->A;
  D = b->config->D;
  f_m = b->feedrate / 60.0;
  l = b->length;
  dt_1 = f_m / A;
  dt_2 = f_m / D;
  dt_m = l / f_m - (dt_1 + dt_2) / 2.0;
  if (dt_m > 0) { // trapezoidal velocity profile
    dt = quantize(dt_1 + dt_2 + dt_m, b->config->tq, &dq);
    dt_m += dq;
    f_m = (2 * l) / (dt_1 + dt_2 + 2 * dt_m);
  }
  else { // triangular velocity profile
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
  b->prof->dt_m = dt_m;
  b->prof->dt_2 = dt_2;
  b->prof->a = a;
  b->prof->d = d;
  b->prof->f = f_m;
  b->prof->dt = (b->type == RAPID ? 60 : dt);
  b->prof->l = l;
}

// returns a valid point for the previous block: origin if the previous
// block is undefined
static point_t point_zero(block_t *b) {
  point_t p0;
  if (b->prev == NULL) {
    p0 = point_new();
    point_xyz(&p0, 0, 0, 0);
  }
  else {
    p0 = b->prev->target;
  }
  return p0;
}

//   ____        _     _ _
//  |  _ \ _   _| |__ | (_) ___
//  | |_) | | | | '_ \| | |/ __|
//  |  __/| |_| | |_) | | | (__
//  |_|    \__,_|_.__/|_|_|\___|
// functions

block_t *block_new(char *line, block_t *prev, struct machine_config *cfg) {
  assert(line);
  block_t *b = malloc(sizeof(block_t));
  assert(b);

  // memory setup
  if (prev) { // if there is a previous block
    memcpy(b, prev, sizeof(block_t));
    b->prev = prev;
    prev->next = b;
  }
  else { // this is the first block
    memset(b, 0, sizeof(block_t));
    b->prev = NULL; // redundant
  }

  // fields that must be calculated
  b->length = 0.0;
  b->target = point_new();
  b->delta = point_new();
  // allocate memory for referenced objects:
  b->prof = malloc(sizeof(block_profile_t));
  assert(b->prof);
  // copy line to b->line
  b->line = malloc(strlen(line) + 1);
  strcpy(b->line, line);
  b->config = cfg;
  b->type = NO_MOTION;
  return b;
}

void block_free(block_t *block) {
  assert(block);
  // only block line and prof if they are not NULL
  if (block->line) free(block->line);
  if (block->prof) free(block->prof);
  free(block);
}

int block_parse(block_t *block) {
  assert(block);
  char *line, *word, *to_free;
  point_t p0;
  int rv = EXIT_SUCCESS;

  // strsep changes the string on which it operates
  // so we make a copy of it. Also, keep track of the original copy
  // pointer so that at the end we can free it (to_free)
  to_free = line = strdup(block->line); // uses malloc internally

  // loop and split line into words
  while ((word  = strsep(&line, " ")) != NULL) {
    // parse each word
    // "x123.9" -> 'X', "123.9"
    rv = block_set_field(block, toupper(word[0]), word + 1);
  }
  free(to_free);

  // inherit from previous block
  p0 = point_zero(block);
  point_modal(&p0, &block->target);
  point_delta(&p0, &block->target, &block->delta);
  block->length = point_dist(&p0, &block->target);

  // compute the fields to be calculated
  if (block->type <= ARC_CCW) {
    block_compute(block);
  }
  return rv;
}

// Calculate lambda value at a given time
data_t block_lambda(block_t *b, data_t t) {
  assert(b);
  data_t dt_1 = b->prof->dt_1;
  data_t dt_2 = b->prof->dt_2;
  data_t dt_m = b->prof->dt_m;
  data_t a = b->prof->a;
  data_t d = b->prof->d;
  data_t f = b->prof->f;
  data_t r;

  if (t < 0) { // negative time
    r = 0.0;
  }
  else if (t < dt_1) { // acceleration
    r = a * pow(t, 2) / 2.0;
    r /= b->prof->l;
  }
  else if (t < (dt_1 + dt_m)) { // maintenance
    r = f * (dt_1 / 2.0 + (t - dt_1));
    r /= b->prof->l;
  }
  else if (t < (dt_1 + dt_m + dt_2)) { // deceleration
    data_t t_2 = dt_1 + dt_m;
    r = f * dt_1 / 2.0 + f * (dt_m + t - t_2) +
        d / 2.0 * (pow(t, 2) + pow(t_2, 2)) - d * t * t_2;
    r /= b->prof->l;
  }
  else { // after ending time
    r = 1.0;
  }
  return r;
}

// interpolate axes positions at a given lambda value:
// x(t) = x_0 + Dx * lambda(t)
point_t  block_interpolate(block_t *b, data_t lambda) {
  assert(b);
  point_t result = point_new();
  point_t p0 = point_zero(b);

  point_x(&result, p0.x + b->delta.x * lambda);
  point_y(&result, p0.y + b->delta.y * lambda);
  point_z(&result, p0.z + b->delta.z * lambda);

  return result;
}

// print block description
void block_print(block_t *b, FILE *out) {
  assert(b);
  assert(out);
  point_t p0 = point_zero(b);
  char *t, *p;

  point_inspect(&b->target, &t);
  point_inspect(&p0, &p);

  fprintf(out, "%03zu: %s -> %s F%7.1f S%7.1f T%2zu (%d)\n", b->n, p, t, b->feedrate, b->spindle, b->tool, b->type);

  // CRUCIAL!!! or you'll have memory leaks!
  free(t);
  free(p);

}

#ifdef BLOCK_MAIN
int main() {
  block_t *b1, *b2;
  data_t t, lambda;
  point_t p = point_new();
  char *p_desc;
  struct machine_config cfg = {.A = 10, .D = 5, .tq = 0.005};

  b1 = block_new("N10 G00 X100 Y100 z100 t3", NULL, &cfg);
  block_parse(b1);
  b2 = block_new("N20 G01 Y120 X110 f1000 s2000", b1, &cfg);
  block_parse(b2);

  block_print(b1, stderr);
  block_print(b2, stderr);

  for (t = 0; t <= b2->prof->dt; t += b2->config->tq) {
    lambda = block_lambda(b2, t);
    p = block_interpolate(b2, lambda);
    printf("%f %f %f %f %f\n", t, lambda, p.x, p.y, p.z);
  }


  block_free(b1);
  block_free(b2);
}
#endif
