#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

typedef struct {
  double x,y,z;
} point_t;

int transform(point_t *pt, point_t *d, double angle) {
  int rv;
  gsl_matrix *T = gsl_matrix_alloc(4,4);
  gsl_vector *p = gsl_vector_alloc(4);
  gsl_vector *p1 = gsl_vector_alloc(4);

  double th = angle / 180.0 * M_PI;
  double sin_th = sin(th);
  double cos_th = cos(th);

  // Fill gsl_vector p with point pt
  gsl_vector_set_all(p, 1);
  gsl_vector_set(p, 0, pt->x);
  gsl_vector_set(p, 1, pt->y);
  gsl_vector_set(p, 2, pt->z);

  gsl_matrix_set_identity(T);
  gsl_matrix_set(T, 0, 0, cos_th);
  gsl_matrix_set(T, 0, 1, -sin_th);
  gsl_matrix_set(T, 0, 3, d->x);
  // Second line
  gsl_matrix_set(T, 1, 0, sin_th);
  gsl_matrix_set(T, 1, 1, cos_th);
  gsl_matrix_set(T, 1, 3, d->y);
  // Third line
  gsl_matrix_set(T, 2, 3, d->z);

  rv = gsl_blas_dgemv(CblasNoTrans, 1, T, p, 0, p1);

  pt->x = gsl_vector_get(p1, 0);
  pt->y = gsl_vector_get(p1, 1);
  pt->z = gsl_vector_get(p1, 2);

  gsl_vector_free(p);
  gsl_vector_free(p1);
  gsl_matrix_free(T);

  return rv;

}


int main() {
  // Input point
  point_t pt = {.x=10, .y=3, .z=7};

  point_t d = {.x=10, .y=0, .z=5}; // Traslate
  double angle = 45.0f; // Rotate

  fprintf(stdout, "pt = (%6.3f - %6.3f - %6.3f)\n", pt.x, pt.y, pt.z);
  fprintf(stdout, "d  = (%6.3f - %6.3f - %6.3f)\n", d.x, d.y, d.z);
  fprintf(stdout, "angle = %6.3f\n", angle);
  int rv = transform(&pt, &d, angle);
  fprintf(stdout, "rotated = (%6.3f - %6.3f - %6.3f)\n", pt.x, pt.y, pt.z);

}