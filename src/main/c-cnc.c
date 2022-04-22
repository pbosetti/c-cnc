#include "../defines.h"
#include "../machine.h"
#include "../block.c"
#include "../point.c"

#define eprintf(...) fprintf (stderr, __VA_ARGS__)

int main(int argc, char const *argv[]) {
  block_t *b0, *b1;
  data_t t, lambda, f, tq;
  char *desc = NULL, *line = NULL;
  point_t *p = NULL;
  machine_t *machine = machine_new("settings.ini");
  if (!machine) {
    eprintf("Error creating machine instance\n");
    exit(EXIT_FAILURE);
  }
  tq = machine_tq(machine);
  eprintf("This is %s v.%s (%s)\n", argv[0], VERSION, BUILD_TYPE);

  // CLI: no arguments: default line
  //      one argument: a double-quoted G-code string 
  if (argc == 1) {
    asprintf(&line, "N20 G01 Y20 X20 f1000 s2000");
  }
  else if (argc == 2) {
    line = strdup(argv[1]);
  }
  else {
    eprintf("No arguments or a single argument (G-code line in \"\")\n");
    return 0;
  }

  // initial setting
  b0 = block_new("N0 G00 f1000 s1000 t1", NULL, machine);
  block_parse(b0);

  // parse block
  b1 = block_new(line, b0, machine);
  if (block_parse(b1)) {
    return 1;
  }

  // print block parameters
  block_print(b1, stderr);
  point_inspect(block_center(b1), &desc);
  eprintf("center: %s, radius: %f, length: %f, dtheta: %f\n", desc, block_r(b1), block_length(b1), block_dtheta(b1));

  // Interpolate block and print a CSV table
  printf("t,lambda,s,f,x,y,z\n");
  for (t = 0; t <= block_dt(b1) + tq/2.0; t += tq) {
    lambda = block_lambda(b1, t, &f);
    p = block_interpolate(b1, lambda);
    printf("%f,%f,%f,%f,%f,%f,%f\n", t, lambda, lambda * block_length(b1), f, point_x(p), point_y(p), point_z(p));
    point_free(p);
  }

  // Cleanup
  block_free(b1);
  block_free(b0);
  machine_free(machine);
  return 0;
}
