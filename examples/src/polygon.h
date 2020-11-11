#include <stdio.h>
#include <stdlib.h>

// double inclusion guard
#ifndef POLYGON_H
#define POLYGON_H

typedef double data_t;
typedef unsigned int index_t;

// coordinate object
typedef struct {
  data_t x, y;
} point_t;

// polygon object
typedef struct {
  index_t id;
  char *name;
  index_t n_sides;
  data_t side_length;
  point_t center;
  data_t angle;
  data_t perimeter, area;
  point_t *vertexes;
} polygon_t;

// Functions

// Create a new polygon with a name, n sides, and len as side length
polygon_t *polygon_new(char *name, size_t n, data_t len);

// calculate perimeter and area of the polygon poly
void polygon_calc(polygon_t *poly);

// print a polygon description
void polygon_print(polygon_t *poly);

// free allocated memory
void polygon_free(polygon_t *poly);



#endif // double inclusion guard