//   ____  _            _    
//  | __ )| | ___   ___| | __
//  |  _ \| |/ _ \ / __| |/ /
//  | |_) | | (_) | (__|   < 
//  |____/|_|\___/ \___|_|\_\
//
#include "block.h"
#include <assert.h>
#include <ctype.h>

typedef struct {
  data_t a, d;             // aceleration and deceleration
  data_t f, l;             // feedrate and length
  data_t dt_1, dt_m, dt_2; // times
  data_t dt;               // timestep
} block_profile_t;

typedef struct block {
  char *line;            // G-code original line
  block_type_t type;     // Block type
  size_t n;              // Block number
  size_t tool;           // Tool number
  data_t feedrate;       // Feedrate (mm/min)
  data_t spindle;        // Spindle rate (rpm)
  point_t *target;       // Target position at the end of the block
  data_t length;         // Block length
  point_t *delta;        // Block projections
  point_t *center;       // Arc center
  data_t i, j, r;        // Arc positions
  data_t theta0, dtheta; // Arc angles
  int ij;                // 1 if block uses I and J, 0 if it uses R
  block_profile_t *prof; // Velocity profile
  // Linked list pointers:
  struct block *next;    // This allows b->next->line
  struct block *prev;
  machine_t *config;
} block_t;

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
  q = ((size_t)(t / tq) + 1) * tq;
  *dq = q - t;
  return q;
}

// returns a valid point for the previous block: origin if the previous
// block is undefined
static point_t *point_zero(block_t *b) {
  point_t *p0;
  if (b->prev == NULL) {
    p0 = point_new();
    point_set_xyz(p0, 0, 0, 0);
  } else {
    p0 = b->prev->target;
  }
  return p0;
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
    point_set_x(b->target, atof(arg));
    break;
  case 'Y':
    point_set_y(b->target, atof(arg));
    break;
  case 'Z':
    point_set_z(b->target, atof(arg));
    break;
  case 'I':
    b->i = atol(arg);
    b->ij = 1;
    break;
  case 'J':
    b->j = atol(arg);
    b->ij = 1;
    break;
  case 'R':
    b->r = atol(arg);
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
    return 1;
  }
  if (b->r && b->ij) {
    fprintf(stderr, "ERROR: Cannot mix R and IJ\n");
    return 1;
  } else {
    return 0;
  }
}

static int block_arc(block_t *b) {
  // we need center, radius, arc, and length
  data_t x0, y0, z0, xc, yc, xf, yf, zf;
  point_t *p0 = point_zero(b);
  int arcsgn = (b->type == ARC_CCW ? 1 : -1);
  x0 = point_x(p0);
  y0 = point_y(p0);
  z0 = point_z(p0);
  xf = point_x(b->target);
  yf = point_y(b->target);
  zf = point_z(b->target);

  if (b->ij) { // R is not given
    data_t r, r2;
    r = hypot(b->i, b->j);
    b->r = r;
    xc = x0 + b->i;
    yc = y0 + b->j;
    r2 = hypot(xf - xc, yf - yc);
    if (fabs(r - r2) > machine_error(b->config)) {
      fprintf(stderr, "Arc endpoint mismatch (error %f)\n", r - r2);
      return 1;
    }
  } else { // R is given
    data_t dx = point_x(b->delta);
    data_t dy = point_y(b->delta);
    data_t r = b->r;
    data_t dxy2 = pow(dx, 2) + pow(dy, 2);
    data_t sq = sqrt(-pow(dy, 2) * dxy2 * (dxy2 - 4 * r * r));
    // Signs table:
    // sign(r) | CW(-1) | CCW(+1)
    //      -1 |    +   |   -
    //      +1 |    -   |   +
    int s = (r > 0) - (r < 0);
    s *= arcsgn;
    xc = x0 + (dx - s * sq / dxy2) / 2.0;
    yc = y0 + dy / 2.0 + s * (dx * sq) / (2 * dy * dxy2);
  }

  point_set_x(b->center, xc);
  point_set_y(b->center, yc);
  b->theta0 = atan2(y0 - yc, x0 - xc);
  b->dtheta = atan2(yf - yc, xf - xc) - b->theta0;
  // net angle: complement to 2PI if negative
  if (b->dtheta < 0)
    b->dtheta = 2 * M_PI + b->dtheta;
  // if CCW, take the negative complement
  if (b->type == ARC_CW)
    b->dtheta = -(2 * M_PI - b->dtheta);
  // helix length
  b->length = hypot(zf - z0, b->dtheta * b->r);
  // from now on, it's safer to drop the sign of radius
  b->r = fabs(b->r);
  return 0;
}

// compute velocity profile for the block
static void block_compute(block_t *b) {
  assert(b);
  data_t A, D, a, d;
  data_t dt, dt_1, dt_2, dt_m, dq;
  data_t f_m, l;

  A = machine_A(b->config);
  D = machine_D(b->config);
  f_m = b->feedrate / 60.0;
  l = b->length;
  dt_1 = f_m / A;
  dt_2 = f_m / D;
  dt_m = l / f_m - (dt_1 + dt_2) / 2.0;
  if (dt_m > 0) { // trapezoidal velocity profile
    dt = quantize(dt_1 + dt_2 + dt_m, machine_tq(b->config), &dq);
    dt_m += dq;
    f_m = (2 * l) / (dt_1 + dt_2 + 2 * dt_m);
  } else { // triangular velocity profile
    dt_1 = sqrt(2 * l / (A + pow(A, 2) / D));
    dt_2 = dt_1 * A / D;
    dt = quantize(dt_1 + dt_2, machine_tq(b->config), &dq);
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

//   ____        _     _ _
//  |  _ \ _   _| |__ | (_) ___
//  | |_) | | | | '_ \| | |/ __|
//  |  __/| |_| | |_) | | | (__
//  |_|    \__,_|_.__/|_|_|\___|
// functions

block_t *block_new(char *line, block_t *prev, machine_t *cfg) {
  assert(line);
  block_t *b = calloc(sizeof(block_t), 1);
  assert(b);

  // memory setup
  if (prev) { // if there is a previous block
    memcpy(b, prev, sizeof(block_t));
    b->prev = prev;
    prev->next = b;
  } else { // this is the first block
    memset(b, 0, sizeof(block_t));
    b->prev = NULL; // redundant
  }

  // reset, non-modal!
  b->i = b->j = b->ij = b->r = 0.0;

  // fields that must be calculated
  b->length = 0.0;
  b->target = point_new();
  b->delta = point_new();
  b->center = point_new();
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
  if (block->line)
    free(block->line);
  if (block->prof)
    free(block->prof);
  point_free(block->target);
  point_free(block->delta);
  free(block);
}

int block_parse(block_t *block) {
  assert(block);
  char *line, *word, *to_free;
  point_t *p0;
  int rv = EXIT_SUCCESS;

  // strsep changes the string on which it operates
  // so we make a copy of it. Also, keep track of the original copy
  // pointer so that at the end we can free it (to_free)
  to_free = line = strdup(block->line); // uses malloc internally

  // loop and split line into words
  while ((word = strsep(&line, " ")) != NULL) {
    // parse each word
    // "x123.9" -> 'X', "123.9"
    rv += block_set_field(block, toupper(word[0]), word + 1);
  }
  free(to_free);

  // inherit from previous block
  p0 = point_zero(block);
  point_modal(p0, block->target);
  point_delta(p0, block->target, block->delta);
  block->length = point_dist(p0, block->target);

  switch (block->type) {
  case LINE:
    block_compute(block);
    break;
  case ARC_CW:
  case ARC_CCW:
    block_arc(block);
    // centripetal acc = v^2/r (in mm/min)
    block->feedrate =
        MIN(block->feedrate, sqrt(machine_A(block->config) * block->r) * 60);
    fprintf(stderr, "Curve feedrate: %f\n", block->feedrate);
    block_compute(block);
    break;
  default:
    break;
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
  } else if (t < dt_1) { // acceleration
    r = a * pow(t, 2) / 2.0;
    r /= b->prof->l;
  } else if (t < (dt_1 + dt_m)) { // maintenance
    r = f * (dt_1 / 2.0 + (t - dt_1));
    r /= b->prof->l;
  } else if (t < (dt_1 + dt_m + dt_2)) { // deceleration
    data_t t_2 = dt_1 + dt_m;
    r = f * dt_1 / 2.0 + f * (dt_m + t - t_2) +
        d / 2.0 * (pow(t, 2) + pow(t_2, 2)) - d * t * t_2;
    r /= b->prof->l;
  } else { // after ending time
    r = 1.0;
  }
  return r;
}

// interpolate axes positions at a given lambda value:
// x(t) = x_0 + Dx * lambda(t)
point_t *block_interpolate(block_t *b, data_t lambda) {
  assert(b);
  point_t *result = point_new();
  point_t *p0 = point_zero(b);

  if (b->type == LINE) {
    point_set_x(result, point_x(p0) + point_x(b->delta) * lambda);
    point_set_y(result, point_y(p0) + point_y(b->delta) * lambda);
  } else if (b->type == ARC_CCW || b->type == ARC_CW) {
    point_set_x(result, point_x(b->center) +
                            b->r * cos(b->theta0 + b->dtheta * lambda));
    point_set_y(result, point_y(b->center) +
                            b->r * sin(b->theta0 + b->dtheta * lambda));
  }
  point_set_z(result, point_z(p0) + point_z(b->delta) * lambda);

  return result;
}

// print block description
void block_print(block_t *b, FILE *out) {
  assert(b);
  assert(out);
  point_t *p0 = point_zero(b);
  char *t, *p;

  point_inspect(b->target, &t);
  point_inspect(p0, &p);

  fprintf(out, "%03lu: %s -> %s F%7.1f S%7.1f T%2lu (%d)\n", b->n, p, t,
          b->feedrate, b->spindle, b->tool, b->type);

  // CRUCIAL!!! or you'll have memory leaks!
  free(t);
  free(p);
}

data_t block_dt(block_t *block) { return block->prof->dt; }

data_t block_dtheta(block_t *block) { return block->dtheta; }

point_t *block_center(block_t *block) { return block->center; }