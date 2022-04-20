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

// The following code is very repetitive:
#ifdef NOT_DRY
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
#else
// DRY the code:
#define point_accessor(axis, bitmask)               \
  void point_set_##axis(point_t *p, data_t value) { \
    assert(p);                                      \
    p->axis = value;                                \
    p->s |= bitmask;                                \
  }                                                 \
  data_t point_##axis(point_t *p) {                 \
    assert(p);                                      \
    return p->axis;                                 \
  }

point_accessor(x, X_SET)
point_accessor(y, Y_SET)
point_accessor(z, Z_SET)
#endif


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

#define FIELD_LENGTH 8
void point_inspect(point_t *p, char **desc) {
  char str_x[FIELD_LENGTH+1], str_y[FIELD_LENGTH+1], str_z[FIELD_LENGTH+1];
  if (p->s & X_SET) { // defined
    snprintf(str_x, FIELD_LENGTH, "%*.3f", FIELD_LENGTH, p->x);
  }
  else { // not defined
    snprintf(str_x, FIELD_LENGTH, "%*s", FIELD_LENGTH, "-");
  }

  if (p->s & Y_SET) { // defined
    snprintf(str_y, FIELD_LENGTH, "%*.3f", FIELD_LENGTH, p->y);
  }
  else { // not defined
    snprintf(str_y, FIELD_LENGTH, "%*s", FIELD_LENGTH, "-");
  }

  if (p->s & Z_SET) { // defined
    snprintf(str_z, FIELD_LENGTH, "%*.3f", FIELD_LENGTH, p->z);
  }
  else { // not defined
    snprintf(str_z, FIELD_LENGTH, "%*s", FIELD_LENGTH, "-");
  }
  asprintf(desc, "[%s %s %s]", str_x, str_y, str_z);
}
#undef FIELD_LENGTH



//   _____ _____ ____ _____   __  __       _       
//  |_   _| ____/ ___|_   _| |  \/  | __ _(_)_ __  
//    | | |  _| \___ \ | |   | |\/| |/ _` | | '_ \
//    | | | |___ ___) || |   | |  | | (_| | | | | |
//    |_| |_____|____/ |_|   |_|  |_|\__,_|_|_| |_|
// 
#ifdef POINT_MAIN
int main() {
  point_t *p0, *p1, *p2;
  char *desc;
  p0 = point_new();
  p1 = point_new();
  p2 = point_new();

  point_xyz(p0, 0, 0, 0);
  point_set_x(p1, 100);
  point_set_y(p1, 100);
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