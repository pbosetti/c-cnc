#include "../defines.h"
#include "../block.h"
#include "../machine.h"

int main() {
  block_t *b1, *b2;
  data_t t, lambda;
  machine_t *cfg = machine_new("settings.ini");
  if (!cfg) {
    return 1;
  }
  point_t *p = point_new();
  // char *p_desc;

  b1 = block_new("N10 G00 X100 Y100 z100 t3", NULL, cfg);
  block_parse(b1);
  b2 = block_new("N20 G01 Y120 X110 f1000 s2000", b1, cfg);
  block_parse(b2);

  block_print(b1, stderr);
  block_print(b2, stderr);
  printf("t lambda x y z\n");
  for (t = 0; t <= block_dt(b2); t += machine_tq(cfg)) {
    lambda = block_lambda(b2, t);
    p = block_interpolate(b2, lambda);
    printf("%f %f %f %f %f\n", t, lambda, point_x(p), point_y(p), point_z(p));
  }

  block_free(b1);
  block_free(b2);
  return 0;
}