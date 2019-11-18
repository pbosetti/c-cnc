#include <stdlib.h>
#include <stdio.h>

typedef struct {
  struct {
    double x, y;
  } point;
  void *next;
  void *prev;
} segment_t;

typedef struct {
  segment_t *first;
  segment_t *last;
} poly_t;

segment_t *segment_new(double x, double y) {
  /* to be completed */
}

poly_t *poly_new(double x, double y) {
  /* to be completed */
}

void poly_add(poly_t *poly, segment_t *seg) {
  /* to be completed */
}

void poly_insert_at(poly_t *poly, segment_t *seg, size_t pos) {
  /* to be completed */
}

void poly_free(poly_t *poly) {
  /* to be completed */
}

int main(int argc, char const *argv[]) {
  size_t i;
  segment_t *seg;
  double x[10] = {1, 3, 8, 2, 6, 4, 10, 5, 5, 1};
  double y[10] = {6, 2, 1, 2, 8, 8, 3, 1, 5, 7};
  poly_t *poly = poly_new(0, 0);
  for (i = 0; i < 10; i++) {
    segment_t *s = segment_new(x[i], y[i]);
    poly_add(poly, s);
  }

  // forward
  /* to be completed: loop and print vertex positions and index from first to last 
    format must be:
    1: {1, 6}
    2: {3, 2}
    etc
  */

  // Backward
  /* to be completed: loop and print vertex positions and index from last to first
    format must be:
    10: {1, 7}
     9: {5, 5}
    etc
  */

  poly_free(poly);
  return 0;
}
