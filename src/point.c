#include "point.h"

point_t point_new() {
  point_t p = {0, 0, 0};
  p.s = 0;
}

#define X_SET '\1'
#define Y_SET '\2'
#define Z_SET '\4'
#define ALL_SET '\7'

// BITMASK
// p.s: xxxx xxxx
//   1: 0000 0001
// --------------
// OR:  xxxx xxx1
void point_x(point_t *p, data_t v) {
  p->x = v;
  p->s = p->s | X_SET;
}

void point_y(point_t *p, data_t v) {
  p->y = v;
  p->s |= Y_SET;
}

void point_z(point_t *p, data_t v) {
  p->z = v;
  p->s |= Z_SET;
}

void point_xyz(point_t *p, data_t x, data_t y, data_t z) {
  p->x = x;
  p->y = y;
  p->z = z;
  p->s = ALL_SET;
}

// calculate distance between p1 and p2 (always positive)
data_t point_dist(point_t *p1, point_t *p2) {
  return sqrt(
    pow(p2->x - p1->x, 2) + 
    pow(p2->y - p1->y, 2) +
    pow(p2->z - p1->z, 2)
  );
}

// calculates the three projections of a segment
void point_delta(point_t *p1, point_t *p2, point_t *delta) {
  assert(p1->s == ALL_SET && p2->s == ALL_SET);
  point_xyz(delta, p2->x - p1->x, p2->y - p1->y, p2->z - p1->z);
}

// modal behavior: copy p1 values into undefined p2 values
void point_modal(point_t *p1, point_t *p2) {
  if ( !(p2->s & X_SET) && (p1->s & X_SET)) {
    point_x(p2, p1->x);
  }
  if ( !(p2->s & Y_SET) && (p1->s & Y_SET)) {
    point_y(p2, p1->y);
  }
  if ( !(p2->s & Z_SET) && (p1->s & Z_SET)) {
    point_Z(p2, p1->z);
  }
}


// provide a description of the point
// description as [1.000 - 2.345]
void point_inspect(point_t *p, char **desc) {
  if (p == NULL) { // [       -        -        -]
    asprintf(desc, "[%8s %8s %8s]", "-", "-", "-");
  } else {
    char str_x[9], str_y[9], str_z[9];
    if (p->s & X_SET) {
      sprintf(str_x, "%8.3f", p->x);
    } else { // X is undefined 
      sprintf(str_x, "%8s", "-");
    }
    if (p->s & Y_SET) {
      sprintf(str_y, "%8.3f", p->y);
    } else { // Y is undefined 
      sprintf(str_y, "%8s", "-");
    }
    if (p->s & Z_SET) {
      sprintf(str_z, "%8.3f", p->z);
    } else { // Z is undefined 
      sprintf(str_z, "%8s", "-");
    }
    // assemble descriptions as
    // [  01.234        -   23.456]
    asprintf(desc, "[%s %s %s]", str_x, str_y, str_z);
  }
}