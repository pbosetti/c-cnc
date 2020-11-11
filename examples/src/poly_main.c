#include "polygon.h"

int main() {
  polygon_t *p1, *p2;

  p1 = polygon_new("square", 4, 2.0);
  p2 = polygon_new("pentagon", 5, 1.5);

  printf("POLYGON 1:\n");
  polygon_print(p1);
  printf("POLYGON 2:\n");
  polygon_print(p2);

  polygon_free(p1);
  polygon_free(p2);

  return 0;
}