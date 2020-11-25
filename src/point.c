#include "point.h"

// create a new point with all coordinates unset
point_t point_new() {
  point_t p = {0,0,0};
  p.s = 0;
  return p;
}

// Bitmask meanings
#define X_SET '\1'   // binary 0000 0001
#define Y_SET '\2'   // binary 0000 0010
#define Z_SET '\4'   // binary 0000 0100
#define ALL_SET '\7' // binary 0000 0111


// BITMASK:
// p.s: xxxx xxxx
//   1: 0000 0001
// --------------
//  OR: xxxx xxx1
void point_x(point_t *p, data_t v) {
  assert(p);
  p->x = v;
  p->s = p->s | X_SET;
}

// BITMASK:
// p.s: xxxx xxxx
//   2: 0000 0010
// --------------
//  OR: xxxx xx1x
void point_y(point_t *p, data_t v) {
  assert(p);
  p->y = v;
  p->s |= Y_SET;
}

// BITMASK:
// p.s: xxxx xxxx
//   4: 0000 0100
// --------------
//  OR: xxxx x1xx
void point_z(point_t *p, data_t v) {
  assert(p);
  p->z = v;
  p->s |= Z_SET;
}

// BITMASK:
// p.s: xxxx xxxx
//   7: 0000 0111
// --------------
//  OR: xxxx x111
void point_xyz(point_t *p, data_t x, data_t y, data_t z) {
  assert(p);
  p->x = x;
  p->y = y;
  p->z = z;
  p->s |= ALL_SET;
}

data_t point_dist(point_t *p1, point_t *p2) {
  // ensure that p1 and p2 have all fields set
  assert(p1->s == ALL_SET && p2->s == ALL_SET);
  return sqrt(
    pow(p2->x - p1->x, 2) +
    pow(p2->y - p1->y, 2) +
    pow(p2->z - p1->z, 2) 
  );
}

void point_delta(point_t *p1, point_t *p2, point_t *delta) {
  // ensure that p1 and p2 have all fields set and delta is not NULL
  assert(p1->s == ALL_SET && p2->s == ALL_SET && delta != NULL);
  point_xyz(delta, 
    p2->x - p1->x,
    p2->y - p1->y,
    p2->z - p1->z
  );
}

// BITMASK:
// p.s: xxxx x001
//   2: 0000 0010 Y_SET
// --------------
// AND: 0000 0000
void point_modal(point_t *p1, point_t *p2) {
  // ensure that p1 and p2 have all fields set
  assert(p1 != NULL && p2 != NULL);
  // (p2->s & X_SET) is 0 (false) unless the last bit of p2->s is set
  if ( !(p2->s & X_SET) && (p1->s & X_SET)) {
    point_x(p2, p1->x);
  }
  if ( !(p2->s & Y_SET) && (p1->s & Y_SET)) {
    point_y(p2, p1->y);
  }
  if ( !(p2->s & Z_SET) && (p1->s & Z_SET)) {
    point_z(p2, p1->z);
  }
}

// fill a given string pointer with a description of the 
// point
// How to use it:
//   char *desc;
//   point_desc(pt, &desc);
//   printf("Point: %s\n", desc);
void point_inspect(point_t *p, char **desc) {
  assert(p);
  // local (on stack) temporary strings
  char str_x[9], str_y[9], str_z[9];
  // fill local strings with coords descriptions
  if (p->s & X_SET) {
    sprintf(str_x, "%8.3f", p->x);
  }
  else {
    sprintf(str_x, "%8s", "-");
  }
  if (p->s & Y_SET) {
    sprintf(str_y, "%8.3f", p->y);
  }
  else {
    sprintf(str_y, "%8s", "-");
  }
  if (p->s & Z_SET) {
    sprintf(str_z, "%8.3f", p->z);
  }
  else {
    sprintf(str_z, "%8s", "-");
  }
  // allocates on stack a string desc and prints into it 
  // the local strings
  asprintf(desc, "[%s %s %s]", str_x, str_y, str_z);
}


// example main() - it is only compiled if the 
// macro POINT_MAIN is defined, either in the source files
// or - more conveniently - on the command line of the
// compiler, with the switch -DPOINT_MAIN
#ifdef POINT_MAIN
int main() {
  point_t p1 = point_new();
  point_t p2 = point_new();
  point_t delta = point_new();
  char *desc_p1, *desc_p2;

  point_xyz(&p1, 0, 0, 0);
  point_x(&p2, 100);
  point_z(&p2, 100);

  point_inspect(&p1, &desc_p1);
  point_inspect(&p2, &desc_p2);
  printf("p1: %s, p2: %s\n", desc_p1, desc_p2);

  point_modal(&p1, &p2);
  point_inspect(&p1, &desc_p1);
  point_inspect(&p2, &desc_p2);
  printf("p1: %s, p2: %s\n", desc_p1, desc_p2);

  printf("Distance p1-p2: %f\n", point_dist(&p1, &p2));
  point_delta(&p1, &p2, &delta);
  point_inspect(&delta, &desc_p2);
  printf("Delta p1-p2: %s\n", desc_p2);

  return 0;
}
#endif