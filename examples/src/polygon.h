#include <stdio.h>
#include <stdlib.h>

typedef double data_t;
typedef unsigned int index_t;

typedef struct {
  data_t x, y;
} point_t;

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

