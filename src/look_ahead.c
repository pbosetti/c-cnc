#include "look_ahead.h"
#include <math.h>

#define CRITICAL_ANGLE_FACTOR 2 // 2=90°, 4=45°, ...

typedef struct {
  data_t si, s1, s2, sf;
  data_t vi, v1, v2, vf;
} segment_t;

typedef struct _look_ahead_strct {
  block_t *start;
  block_t *stop;
  segment_t **segments;
  index_t n_segments;
  index_t n_blocks;
} look_ahead_t;

// from speed to distance, with constnt acceleration a
static inline data_t f_s(data_t s, data_t s0, data_t f0, data_t a) {
  return sqrt(pow(f0 / 60, 2) + 2 * a * (s - s0)) * 60;
}

// from distance to speed, with constant acceleration a
static inline data_t s_f(data_t f, data_t s0, data_t f0, data_t a) {
  return s0 + (pow(f / 60, 2) - pow(f0 / 60, 2)) / (2 * a);
}

// saturate a value
static inline void sat(data_t *v, data_t u, data_t l) {
  *v = fmin(*v, u);
  *v = fmax(*v, l);
}

// nominal speed at the corner of b with previous block
static inline data_t speed_at_corner(block_t *b) {
  data_t f = (1 + cos(CRITICAL_ANGLE_FACTOR * b->angle)) / 2;
  sat(&f, 1, 0);
  if (b->prev)
    return fmin(b->prev->feedrate, b->feedrate) * f;
  else
    return 0.;
}

// calculate trapezoidal points
static void segment_points(segment_t *s, data_t a, data_t d, data_t f) {
  data_t v0 = s->vi;
  data_t vf = s->vf;
  data_t s0 = s->si;
  data_t sf = s->sf;
  data_t vmax =
      sqrt((d * pow(v0 / 60, 2) + a * (2 * d * (-s0 + sf) + pow(vf / 60, 2))) /
           (a + d)) *
      60;
  if (vmax > f) { // trapezoid
    s->s1 = s_f(f, s0, v0, a);
    s->s2 = s->sf - s_f(f, 0, vf, d);
    s->v1 = f;
    s->v2 = f;
  } else { // triangle
    s->s1 =
        fmin(sf, (2 * a * s0 + 2 * d * sf - pow(v0 / 60, 2) + pow(vf / 60, 2)) /
                     (2. * (a + d)));
    s->s2 = s->s1;
    s->v1 = vmax;
    s->v2 = vmax;
  }
}

static void look_ahead_free_segments(look_ahead_t *lah) {
  register index_t i = 0;
  for (i = 0; i < lah->n_segments; i++)
    free(lah->segments[i]);
}

//   ____        _     _ _
//  |  _ \ _   _| |__ | (_) ___
//  | |_) | | | | '_ \| | |/ __|
//  |  __/| |_| | |_) | | | (__
//  |_|    \__,_|_.__/|_|_|\___|

look_ahead_t *look_ahead_start(look_ahead_t *lah, block_t *start) {
  assert(start);
  if (lah == NULL) {
    lah = malloc(sizeof(look_ahead_t));
    lah->segments = NULL;
  }
  else { // free all existing segments
    look_ahead_free_segments(lah);
  }
  lah->start = start;
  lah->n_segments = 0;
  lah->n_blocks = 0;
  return lah;
}

void look_ahead_stop(look_ahead_t *lah, block_t *stop) {
  register index_t i, j;
  block_t *b;
  data_t s;          // curvilinear abscissa
  data_t l;          // block length
  data_t f, f0;      // G-code feedrate of current and previous block
  data_t A, D;       // Nominal acceleration and deceleration
  data_t nom_speed;  // nominal speed at the beginning of block (including corner deceleration)
  data_t real_speed; // max speed attainable from beginning of segment
  data_t next_speed; // nominal speed at the end of block
  int brk = 0;       // flag

  // set current block as closing one
  lah->stop = stop;

  // count blocks
  b = lah->start;
  i = 0;
  do {
    i++;
  } while ((b = b->next) != lah->stop);

  // allocate containers
  lah->n_blocks = i;
  lah->segments = realloc(lah->segments, lah->n_blocks * sizeof(segment_t *));

  b = lah->start;
  i = 0, j = 0, s = 0, f0 = 0;
  l = b->length;
  f = b->feedrate;
  A = b->config->A;
  D = b->config->D;
  lah->segments[0] = malloc(sizeof(segment_t));
  lah->segments[0]->si = 0;
  lah->segments[0]->vi = 0;
  for (i = 0; i < lah->n_blocks; i++) {
    f = b->feedrate;
    l = b->length;
    nom_speed = speed_at_corner(b);
    real_speed = f_s(s, lah->segments[j]->si, lah->segments[j]->vi, A);
    if (i == lah->n_blocks - 1)
      next_speed = 0;
    else
      next_speed = b->next->feedrate;
    // fprintf(stderr, "at %d, s=%f f0=%f, f=%f, sac=%f, angle=%f, v=%f", b->n,
    //         s + l, f0, f, nom_speed, b->angle / M_PI * 180, real_speed);

    if (i > 0) { // don't break on first
      if (f != f0) {
        brk = 1;
      } else if (nom_speed <
                 fmin(real_speed, f * (1 - 1E-6))) { // rounding error catch
        brk = 2;
      } else {
        brk = 0;
      }
      if (f_s(l, 0, f, -D) > next_speed) {
        brk = 0; //
        // fprintf(stderr, "TOO SHORT, NOT BREAKING!\n");
      }
    }

    if (brk) { // close segment AT THE END OF PREVIOUS BLOCK
      lah->segments[j]->vf = fmin(nom_speed, real_speed);
      lah->segments[j]->sf = s;
      segment_points(lah->segments[j], A, D, f0);
      lah->segments[j + 1] = malloc(sizeof(segment_t));
      lah->segments[j + 1]->si = lah->segments[j]->sf;
      lah->segments[j + 1]->vi = lah->segments[j]->vf;
      j++;
      // fprintf(stderr, " BREAK %d\n", brk);
    } else {
      // fprintf(stderr, "\n");
    }
    f0 = f;
    s += l;
    b = b->next;
  }
  lah->segments[j]->vf = 0; // fmin(nom_speed, real_speed);
  lah->segments[j]->sf = s;
  segment_points(lah->segments[j], A, D, f0);
  lah->n_segments = j + 1;
}

void look_ahead_update_blocks(look_ahead_t *lah) {
  int i = 0, j = 0;
  block_t *b = lah->start;
  data_t A = b->config->A;
  data_t D = b->config->D;
  data_t s = 0.;
  for (i = 0; i < lah->n_blocks; i++) {
    // initial feedrate
    if (b == lah->start) {
      b->feedrate_in = lah->segments[j]->vi;
    } else {
      b->feedrate_in = b->prev->feedrate_out;
    }
    s += b->length;
    // final feedrate
    if (s < lah->segments[j]->s1) {        // block ends during accel
      b->feedrate_out = f_s(b->length, 0, b->feedrate_in, A);
      b->feedrate_max = b->feedrate_out;
    } else if (s < lah->segments[j]->s2) { // block ends during maintenance
      b->feedrate_out = b->feedrate;
      b->feedrate_max = lah->segments[j]->v2;
    } else if (s < lah->segments[j]->sf) { // block ends during deceleration
      b->feedrate_out = f_s(lah->segments[j]->sf, s, lah->segments[j]->vf, D);
      b->feedrate_max = lah->segments[j]->v2;
    } else {                               // block ends at segment
      b->feedrate_out = lah->segments[j]->vf;
      b->feedrate_max = lah->segments[j]->v2;
      j++;
    }
    if (i == lah->n_blocks - 1) {          // last block, end of segments
      b->feedrate_out = 0.;
    }
    block_compute_profile(b);
    b = b->next;
  }
}

void look_ahead_print(look_ahead_t *lah, FILE *dest) {
  index_t i;
  fprintf(dest, "%2s %8s %8s\n", "n", "s", "f");
  for (i = 0; i < lah->n_segments; i++) {
    fprintf(dest, "%2d %8.3f %8.3f\n", i, lah->segments[i]->si,
            lah->segments[i]->vi);
    fprintf(dest, "%2d %8.3f %8.3f\n", i, lah->segments[i]->s1,
            lah->segments[i]->v1);
    fprintf(dest, "%2d %8.3f %8.3f\n", i, lah->segments[i]->s2,
            lah->segments[i]->v2);
    fprintf(dest, "%2d %8.3f %8.3f\n", i, lah->segments[i]->sf,
            lah->segments[i]->vf);
  }
}

void look_ahead_free(look_ahead_t *lah) {
  assert(lah);
  look_ahead_free_segments(lah);
  free(lah->segments);
  free(lah);
}

#ifdef LOOKAHEAD_MAIN
// Compile with:
// clang -DLOOKAHEAD_MAIN src/*.c -Imachine/src -lcairo build/libmachine.a
// build/lua-5.3.5/lib/liblua.a -o lookahead_test

int main() {
  struct machine_config cfg = {.tq = 0.005, .A = 100, .D = 100};
  look_ahead_t *lah = NULL;
  block_t *b0 = block_new("N10 g00 x0 y1 z0", NULL, &cfg);
  block_parse(b0);
  block_t *b1 = block_new("N20 g01 x8 y0 z0 F3000", b0, &cfg);
  block_parse(b1);
  block_t *b2 = block_new("N30 g01 x30 y0 z0", b1, &cfg);
  block_parse(b2);
  block_t *b3 = block_new("N40 g01 x60 y0 z0", b2, &cfg);
  block_parse(b3);
  block_t *b4 = block_new("N50 g01 x100 y20 z0 f6000", b3, &cfg);
  block_parse(b4);
  block_t *b5 = block_new("N60 g01 x120 y30 z0", b4, &cfg);
  block_parse(b5);
  block_t *b6 = block_new("N70 g01 x170 y30 z0", b5, &cfg);
  block_parse(b6);
  block_t *b7 = block_new("N80 g01 x200 y30 z0 f 3000", b6, &cfg);
  block_parse(b7);
  block_t *b8 = block_new("N90 g01 x210 y30 z0 f 3000", b7, &cfg);
  block_parse(b8);
  block_t *b9 = block_new("N100 g01 x220 y30 z0 f 1500", b8, &cfg);
  block_parse(b9);

  lah = look_ahead_start(lah, b1);
  look_ahead_stop(lah, NULL);
  look_ahead_print(lah, stderr);

  if (0) {
    fprintf(stderr, "\n");
    data_t s = 0, ds = 0.1;
    data_t v = 0;
    index_t i;
    fprintf(stdout, "%2s %8s %8s\n", "n", "s", "f");
    for (i = 0; i < lah->n_segments; i++) {
      s = lah->segments[i]->si;
      while (s < lah->segments[i]->s1) {
        v = f_s(s, lah->segments[i]->si, lah->segments[i]->vi, cfg.A);
        fprintf(stdout, "%2d %8.3f %8.3f\n", i, s, v);
        s += ds;
      }
      while (s < lah->segments[i]->s2) {
        fprintf(stdout, "%2d %8.3f %8.3f\n", i, s, lah->segments[i]->v1);
        s += ds;
      }
      while (s < lah->segments[i]->sf) {
        v = f_s(s, lah->segments[i]->s2, lah->segments[i]->v2, -cfg.D);
        fprintf(stdout, "%2d %8.3f %8.3f\n", i, s, v);
        s += ds;
      }
    }
  }

  {
    index_t i = 0;
    block_t *b = lah->start;
    data_t s = 0.;
    look_ahead_update_blocks(lah);
    fprintf(stderr, "\n%2s %8s %8s %8s %8s %8s\n", "b", "s", "fi", "fn", "fm", "fe");
    for (i = 0; i < lah->n_blocks; i++) {
      s += b->length;
      fprintf(stderr, "%2d %8.3f %8.3f %8.3f %8.3f %8.3f\n", i, s, b->feedrate_in,
              b->feedrate, b->feedrate_max, b->feedrate_out);
      b = b->next;
    }
  }

  if (1) {
    block_t *b = lah->start;
    data_t t = 0, tt = 0, lambda;
    point_t point;
    printf("n t tt dt lambda x y z\n");
    do {
      for (;t <= b->prof->dt; t += cfg.tq) {
        lambda = block_lambda(b, t);
        if (lambda > 1) break;
        point = block_interpolate(b, lambda);
        fprintf(stdout, "%2d %f %f %f %f %f %f %f\n", b->n, t, tt, b->prof->dt, block_lambda(b, t),  point.x, point.y, point.z);
        tt += cfg.tq;
      }
      t -= b->prof->dt;
    } while ((b = b->next));
  }

  look_ahead_start(lah, b2);
  look_ahead_stop(lah, b4);

  look_ahead_free(lah);
  return 0;
}

#endif