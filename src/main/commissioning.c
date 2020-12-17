#include "../ccnc.h"
#include <machine.h>
#include <linux/limits.h> // needed for PATH_MAX
#include <libgen.h>       // needed for dirname() 


// argv[0] is the path to the current executable
// e.g. ./bin/commissioning
// so for building the path to the config file
// we need to take the directory part of that path
// and append the name of the config file
int main(int argc, char const **argv) {
  struct machine *m;
  char this_path[PATH_MAX];
  char cfg_path[PATH_MAX];
  int i = 0;
  data_t t = 0, dt;
  data_t tf;

  // check for CL arguments: we expect to have exactly one argument
  // which has to be the time duration of the simulation
  if (argc != 2) {
    fprintf(stderr, "I need a time duration as argument!\n");
    return 1;
  }
  tf = atof(argv[1]);

  // build the path to the config file
  strncpy(this_path, dirname((char *)argv[0]), PATH_MAX);
  sprintf(cfg_path, "%s/config.lua", this_path);

  // create a new machine
  m = machine_new(cfg_path);
  // load the sampling time from the machine configuration
  dt = m->cfg->tq;

  // reset the machine to origin at zero speed
  machine_set_position(m, 0,0,0);
  // step change of the set poont by 1 on each axis
  machine_go_to(m, 1,1,1);

  // monitor the evolution of the response to the change in set point:
  for (i = 0; t <= tf; i++) {
    t = i * dt;
    printf("%4d %8.3f %9.3f %9.3f %9.3f\n", i, t, m->x->x, m->y->x, m->z->x);
    machine_do_step(m, dt);
  }

  machine_free(m);

  return 0;
}
