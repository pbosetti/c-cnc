//   ____       _       _   
//  |  _ \ ___ (_)_ __ | |_ 
//  | |_) / _ \| | '_ \| __|
//  |  __/ (_) | | | | | |_ 
//  |_|   \___/|_|_| |_|\__|

#include "point.h"

// Point object struct
// We are using a bitmask for encoding the coordinates that are left
// undefined.
// 0000 0000 => none set (0)
// 0000 0001 => x is set (1)
// 0000 0010 => y is set (2)
// 0000 0100 => z is set (3)
// 0000 0111 => xyz set (7)
typedef struct point {
  data_t x, y, z;
  uint8_t s;
} point_t;


// Create a new point
point_t *point_new() {
  // calloc also initializes the memory to 0!
  point_t *p = (point_t *)calloc(1, sizeof(point_t));
  return p;
}

// Free the memory
void point_free(point_t *p) {
  free(p);
  p = NULL;
}

// Mnemonics for bitmask settings
#define X_SET '\1'
#define Y_SET '\2'
#define Z_SET '\4'
#define ALL_SET '\7'

// SETTERS

// xxxx xxxx Initial p->s value (unknown)
// 0000 0001 Char value of 1: '\1'
// --------- bitwise or
// xxxx xxx1 Result
void point_set_x(point_t *p, data_t x) {
  p->x = x;
  // | is the bitwise or, & is the bitwise and
  p->s = p->s | X_SET;
}

// xxxx xxxx Initial p->s value (unknown)
// 0000 0010 Char value of 2: '\2'
// --------- bitwise or
// xxxx xx1x Result
void point_set_y(point_t *p, data_t y) {
  p->y = y;
  // | is the bitwise or, & is the bitwise and
  p->s |= Y_SET; // like in a = a + 1 => a += 1
}

// xxxx xxxx Initial p->s value (unknown)
// 0000 0100 Char value of 4: '\4'
// --------- bitwise or
// xxxx x1xx Result (x means "either 0 or 1")
void point_set_z(point_t *p, data_t z) {
  p->z = z;
  // | is the bitwise or, & is the bitwise and
  p->s |= Z_SET; // like in a = a + 1 => a += 1
}

// xxxx xxxx Initial p->s value (unknown)
// 0000 0111 Char value of 7: '\7'
// --------- bitwise or
// xxxx x111 Result (x means "either 0 or 1")
void point_set_xyz(point_t *p, data_t x, data_t y, data_t z) {
  p->x = x;
  p->y = y;
  p->z = z;
  p->s = ALL_SET;
}

// GETTERS
data_t point_x(point_t *p) { return p->x; }
data_t point_y(point_t *p) { return p->y; }
data_t point_z(point_t *p) { return p->z; }

// distance between two points
data_t point_dist(point_t *from, point_t *to) {
  return sqrt(
    pow(to->x - from->x, 2) +
    pow(to->y - from->y, 2) +
    pow(to->z - from->z, 2)
  );
}

// Projections
void point_delta(point_t *from, point_t *to, point_t *delta) {
  point_set_xyz(delta, to->x - from->x, to->y - from->y, to->z - from->z);
}

// Modal behavior: only import coordinates from previous point when these are
// NOT DEFINED in current point and DEFINED in previous point
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

// Write into desc a description of a point
// desc is automatically allocated to the right size.
// it is CALLER RESPONSIBILITY TO FREE desc
void point_inspect(point_t *p, char **desc) {
  char str_x[9], str_y[9], str_z[9];
  if (p->s & X_SET) { // defined
    sprintf(str_x, "%8.3f", p->x);
  }
  else { // not defined
    sprintf(str_x, "%8s", "-");
  }

  if (p->s & Y_SET) { // defined
    sprintf(str_y, "%8.3f", p->y);
  }
  else { // not defined
    sprintf(str_y, "%8s", "-");
  }

  if (p->s & Z_SET) { // defined
    sprintf(str_z, "%8.3f", p->z);
  }
  else { // not defined
    sprintf(str_z, "%8s", "-");
  }
  asprintf(desc, "[%s %s %s]", str_x, str_y, str_z);
}



//   _____ _____ ____ _____   __  __       _       
//  |_   _| ____/ ___|_   _| |  \/  | __ _(_)_ __  
//    | | |  _| \___ \ | |   | |\/| |/ _` | | '_ \
//    | | | |___ ___) || |   | |  | | (_| | | | | |
//    |_| |_____|____/ |_|   |_|  |_|\__,_|_|_| |_|
// Only needed for testing purpose. To enable, compile as:
// clang src/point.c -o point -lm -DPOINT_MAIN
#ifdef POINT_MAIN
int main() {
  point_t *p1, *p2, *p3;
  char *desc;
  // Create three points
  p1 = point_new();
  p2 = point_new();
  p3 = point_new();
  // Set first point to origin
  point_set_xyz(p1, 0, 0, 0);
  // Only set Z and Y of second point
  point_set_x(p2, 100);
  point_set_y(p2, 100);
  point_inspect(p2, &desc);
  printf("Initial p2:         %s\n", desc);
  // Modal
  point_modal(p1, p2);
  point_inspect(p2, &desc);
  printf("After modal p2:     %s\n", desc);
  // Distance
  printf("Distance p1->p2:    %f\n", point_dist(p1, p2));
  // Delta (projections)
  point_delta(p1, p2, p3);
  point_inspect(p3, &desc);
  printf("Projections p1->p2: %s\n", desc);
  // Free the memory
  point_free(p1);
  point_free(p2);
  point_free(p3);
  free(desc);
  return 0;
}
#endif