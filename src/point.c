#include "point.h"
#include <math.h>
#include <string.h>

typedef struct point {
  data_t x, y, z;
  uint8_t s;
} point_t;

point_t *point_new() {
  point_t *p = (point_t *)malloc(sizeof(point_t));
  memset(p, 0, sizeof(point_t));
  return p;
}

void point_free(point_t *p) {
  free(p);
  p = NULL;
}

#define X_SET '\1'
#define Y_SET '\2'
#define Z_SET '\4'
#define ALL_SET '\7'

void point_set_x(point_t *p, data_t value) {
  p->x = value;
  p->s = p->s | X_SET;
}

void point_set_y(point_t *p, data_t value) {
  p->y = value;
  p->s |= Y_SET;
}

void point_set_z(point_t *p, data_t value) {
  p->z = value;
  p->s |= Z_SET;
}

data_t point_x(point_t *p) { return p->x; }

data_t point_y(point_t *p) { return p->y; }

data_t point_z(point_t *p) { return p->z; }

void point_set_xyz(point_t *p, data_t x, data_t y, data_t z) {
  p->x = x;
  p->y = y;
  p->z = z;
  p->s = ALL_SET;
}

data_t point_dist(point_t *from, point_t *to) {
  return sqrt(pow(to->x - from->x, 2) + pow(to->y - from->y, 2) +
              pow(to->z - from->z, 2));
}

void point_delta(point_t *from, point_t *to, point_t *delta) {
  point_set_xyz(delta, to->x - from->x, to->y - from->y, to->z - from->z);
}

void point_modal(point_t *from, point_t *to) {
  if (!(to->s & X_SET) && (from->s & X_SET)) {
    point_set_x(to, from->x);
  }
  if (!(to->s & Y_SET) && (from->s & Y_SET)) {
    point_set_y(to, from->y);
  }
  if (!(to->s & Z_SET) && (from->s & Z_SET)) {
    point_set_z(to, from->z);
  }
}

void point_inspect(point_t *p, char **desc) {
  // local (on stack) temporary strings
  char str_x[9], str_y[9], str_z[9];
  // fill local strings with coords descriptions
  if (p->s & X_SET) {
    sprintf(str_x, "%08.3f", p->x);
  } else {
    sprintf(str_x, "%8s", "-");
  }
  if (p->s & Y_SET) {
    sprintf(str_y, "%08.3f", p->y);
  } else {
    sprintf(str_y, "%8s", "-");
  }
  if (p->s & Z_SET) {
    sprintf(str_z, "%08.3f", p->z);
  } else {
    sprintf(str_z, "%8s", "-");
  }
  // allocates on stack a string desc and prints into it
  // the local strings
  asprintf(desc, "[%s %s %s]", str_x, str_y, str_z);
}

#ifdef POINT_MAIN

int main() {
  point_t *p0, *p1, *p2;
  char *desc;
  p0 = point_new();
  p1 = point_new();
  p2 = point_new();

  point_xyz(p0, 0, 0, 0);
  point_x(p1, 100);
  point_y(p1, 100);
  point_inspect(p1, &desc);
  printf("Initial:     %s\n", desc);
  point_modal(p0, p1);
  point_inspect(p1, &desc);
  printf("After modal: %s\n", desc);

  printf("Distance:    %f\n", point_dist(p0, p1));

  point_delta(p0, p1, p2);
  point_inspect(p2, &desc);
  printf("Delta:       %s\n", desc);

  free(p0);
  free(p1);
  free(p2);
  return 0;
}

#endif