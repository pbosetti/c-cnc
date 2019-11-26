#include "../ccnc.h"


int main(int argc, char const *argv[]) {
  point_t p1, p2, p3, delta;
  char *pdesc;
  
  p1 = point_new();
  p2 = point_new();
  p3 = point_new();
  delta = point_new();

  point_xyz(&p1, 0, 0, 0);
  point_x(&p2, 1.32);
  point_z(&p2, 345.987);
  point_modal(&p1, &p2);
  point_z(&p3, -23.96);
  point_modal(&p2, &p3);
  point_delta(&p2, &p3, &delta);

  point_inspect(&p1, &pdesc);
  printf("p1: %s\n", pdesc);
  point_inspect(&p2, &pdesc);
  printf("p2: %s\n", pdesc);
  point_inspect(&p3, &pdesc);
  printf("p3: %s\n", pdesc);
  point_inspect(&delta, &pdesc);
  printf("delta: %s\n", pdesc);

  printf("||p3-p2||: %f\n", point_dist(&p3, &p2));

  free(pdesc);

  return 0;
}
