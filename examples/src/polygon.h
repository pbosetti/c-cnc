// Lesson of Nov. 11th
#include <stdint.h>
#include <stdlib.h>

// CUSTOM TYPES
// a struct representing a point in 2-D
typedef struct {
  double x, y;
} point_t;

// a struct representing a regular polygon in 2-D
typedef struct {
  size_t id;
  char *name;
  size_t n_sides;
  double side_len;
  double perimeter;
  double area;
  point_t center;
  double angle;
  point_t *vertex;
} polygon_t;
// to use it: polygon_t poly;

// FUNCTIONS
// create a new polygon, returning an allocated pointer to it
// this function calls polygon_calc() for perimeter, area and vertex positions
polygon_t *polygon_new(char *name, size_t n, double len);

// calculate the polygon perimeter, area, and vertex positions
void polygon_calc(polygon_t *poly);

// print out a polygon description
void polygon_print(polygon_t *poly);

// free the memory allocated for the polygon
void polygon_free(polygon_t *poly);