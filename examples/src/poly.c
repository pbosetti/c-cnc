// Lesson of Nov. 12th
// This executable shows usage of code in polygon.c/.h 
// to compile, from the folder examples:
//   gcc src/polygon.c src/poly.c -o poly -lm -g
#include <stdio.h>
#include "polygon.h"


int main(int argc, char const *argv[]) {
  // declare and initialize new polygons
  polygon_t *square = polygon_new("square", 4, 1.5);
  polygon_t *pentagon = polygon_new("pentagon", 5, 1.5);

  // change some values and recalculate
  pentagon->center.x = 1.0;
  pentagon->center.y = 0.0;
  polygon_calc(pentagon);

  // Print polygons descriptions
  polygon_print(square);
  printf("\n");
  polygon_print(pentagon);

  // Free allocated memory
  polygon_free(square);
  polygon_free(pentagon);
  return 0;
}

