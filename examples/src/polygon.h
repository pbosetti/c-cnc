#include <stdint.h>
#include <stdlib.h>

typedef struct {
  double x, y;
} point_t;

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

polygon_t *polygon_new(char *name, size_t n, double len);
void polygon_calc(polygon_t *poly);
void polygon_print(polygon_t *poly);

void polygon_free(polygon_t *poly);