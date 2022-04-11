#include "../defines.h"
#include "../block.h"
#include "../machine.h"

int main() {
  block_t *b1, *b2, *b3, *b4;
  data_t t, lambda, f;
  char *desc;
  machine_t *cfg = machine_new("settings.ini");
  if (!cfg) {
    return 1;
  }
  point_t *p = point_new();
  // char *p_desc;

  b1 = block_new("N10 G00 X90 Y90 z100 t3", NULL, cfg);
  if (block_parse(b1)) {
    return 1;
  }
  b2 = block_new("N20 G01 Y100 X100 f1000 s2000", b1, cfg);
  if (block_parse(b2)) {
    return 1;
  }

  block_print(b1, stderr);
  block_print(b2, stderr);
  printf("t,lambda,f,x,y,z\n");
  // for (t = 0; t <= block_dt(b2); t += machine_tq(cfg)) {
  //   lambda = block_lambda(b2, t, &f);
  //   p = block_interpolate(b2, lambda);
  //   printf("%f,%f,%f,%f,%f,%f\n", t, lambda, f, point_x(p), point_y(p), point_z(p));
  // }

  b3 = block_new("N30 g02 I100 j0 x200 y200", b2, cfg);
  if (block_parse(b3)) {
    return 1;
  }
  for (t = 0; t <= block_dt(b3); t += machine_tq(cfg)) {
    lambda = block_lambda(b3, t, &f);
    p = block_interpolate(b3, lambda);
    printf("%f,%f,%f,%f,%f,%f\n", t, lambda, f, point_x(p), point_y(p), point_z(p));
  }
  block_print(b3, stderr);

  b4 = block_new("N30 g03 x120 y120 R-20", b2, cfg);
  if (block_parse(b4)) {
    return 1;
  }
  block_print(b4, stderr);
  point_inspect(block_center(b4), &desc);
  fprintf(stderr, "center: %s, dtheta: %f\n", desc, block_dtheta(b4));

  block_free(b1);
  block_free(b2);
  block_free(b3);
  block_free(b4);
  return 0;
}