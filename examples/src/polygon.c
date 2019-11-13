// Lesson of Nov. 11th
#include "polygon.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

// a global variable only visible from within this file
// it keeps track of the number of polygons created
// so that the id field of each polygon can be set to its
// creation order
size_t PolygonCount = 0;

polygon_t *polygon_new(char *name, size_t n, double len) {
  //point_t origin = {0, 0};
  point_t origin;
  // allocating mmeory for a new polygon object
  polygon_t *poly = malloc(sizeof(polygon_t));
  if (!poly)
    return NULL;
  origin.x = 0.0;
  origin.y = 0.0;

  // filling fields of polygon
  poly->id = PolygonCount++;
  poly->n_sides = n;
  poly->side_len = len;
  poly->perimeter = 0;
  poly->area = 0;
  poly->center = origin;
  poly->angle = 0;
  // dynamic fields need malloc()
  poly->name = malloc((strlen(name) +1) * sizeof(char));
  poly->vertex = malloc(n * sizeof(point_t));
  //poly->name = name; // ERROR IN COMPILATION
  strncpy(poly->name, name, strlen(name));

  // calculate dependent fields
  polygon_calc(poly);
  return poly;
}

void polygon_calc(polygon_t *poly) {
  double r, a;
  size_t i;
  // perimeter
  poly->perimeter = poly->n_sides * poly->side_len;

  // area
  // general formula: A = 1/4 n l^2 / tan(PI/n)
  poly->area = 1 / 4.0 * poly->n_sides * poly->side_len * poly->side_len / tan(M_PI / poly->n_sides);

  // vertex positions
  // using the circumscribed circle of radius r
  r = poly->side_len / (2.0 * sin(M_PI / poly->n_sides));
  for (i = 0; i < poly->n_sides; i++) {
    // one vertex on the circle every angle a
    a = i * 2 * M_PI / poly->n_sides + poly->angle;
    poly->vertex[i].x = r * cos(a) + poly->center.x;
    poly->vertex[i].y = r * sin(a) + poly->center.y;
  }
}

void polygon_print(polygon_t *poly) {
  size_t i;
  printf("Polygon #%ld: %s\n", poly->id, poly->name);
  printf("Center: [%f, %f]\n", poly->center.x, poly->center.y);
  printf("Perimeter: %f\n", poly->perimeter);
  printf("Area: %f\n", poly->area);
  for (i = 0; i < poly->n_sides; i++) {
    printf("  %ld: [%f, %f]\n", i, poly->vertex[i].x, poly->vertex[i].y);
  }
}

void polygon_free(polygon_t *poly) {
  // first: free every dynamically allocated pointer in the 
  // polygon_t structure during initialization in polygon_new():
  free(poly->name);
  free(poly->vertex);
  // then: free the whole struct:
  free(poly);
}