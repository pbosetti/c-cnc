

#include "polygon.h"
#include <string.h>
#include <math.h>

size_t PolygonCount = 0;

polygon_t *polygon_new(char *name, size_t n, data_t len) {
  polygon_t *poly;

  poly = malloc(sizeof(polygon_t));
  if (!poly) {
    return NULL;
  }
  poly->n_sides = n;
  poly->side_length = len;
  poly->perimeter = 0;
  poly->area = 0;
  poly->center.x = 0.0;
  poly->center.y = 0.0;
  poly->angle = 0.0;
  poly->id = ++PolygonCount;
  // in C, strings are null-terminated!
  poly->name = malloc((strlen(name) + 1) * sizeof(char));
  if (!poly->name) return NULL;
  strncpy(poly->name, name, strlen(name));
  poly->vertexes = malloc(n * sizeof(point_t));
  if (!poly->vertexes) return NULL;

  // update values to be calculated
  polygon_calc(poly);
  
  return poly;
}

void polygon_calc(polygon_t *poly) {
  data_t r, a;
  size_t i;

  // calculate perimeter
  poly->perimeter = poly->n_sides * poly->side_length;

  // calculate area
  // general formula: A = 1/4 n l^2 / tan(PI/n)
  poly->area = 1 / 4.0 * poly->n_sides * pow(poly->side_length, 2) / tan(M_PI / poly->n_sides);
  
  // calculate vertexes positions
  // use the circumscribed circle of radius r
  r = poly->side_length / (2.0 * sin(M_PI / poly->n_sides));
  for (i = 0; i < poly->n_sides; i++) {
    a = i * 2 * M_PI / poly->n_sides + poly->angle;
    poly->vertexes[i].x = r * cos(a) + poly->center.x;
    poly->vertexes[i].y = r * sin(a) + poly->center.y;
  }
}

void polygon_print(polygon_t *poly) {
  index_t i;

  printf("Polygon #%d: %s\n", poly->id, poly->name);
  printf("Center: [%f %f]\n", poly->center.x, poly->center.y);
  printf("Perimeter: %f\n", poly->perimeter);
  printf("Area: %f\n", poly->area);
  for (i = 0; i < poly->n_sides; i++) {
    printf("  %d: [%f %f]\n", i, poly->vertexes[i].x, poly->vertexes[i].y);
  }
}

void polygon_free(polygon_t *poly) {
  free(poly->name);
  free(poly->vertexes);
  free(poly);
}