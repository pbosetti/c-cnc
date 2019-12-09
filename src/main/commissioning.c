#include <stdio.h>
#include <machine.h>

int main(int argc, char const *argv[]) {
  data_t t = 0;
  data_t tf = atof(argv[1]);
  struct machine *m = machine_new("bin/config.lua");

  machine_reset(m);
  machine_go_to(m, 1, 1, 1);
  for (t = 0; t <= tf; t += m->cfg->tq) {
    printf("%f %f %f %f\n", t, m->x->x, m->y->x, m->z->x);
    machine_do_step(m, m->cfg->tq);
  }
  machine_free(m);
  return 0;
}
