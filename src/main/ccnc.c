#include "../ccnc.h"
#include "../program.h"
#include <machine.h>

// callbacks
void print_block_descr(block_t *b, void*userdata) {
  block_print(b, stderr);
  printf("#n type t_time b_time error x y z mx my mz\n");
}

block_ctrl_t time_loop(block_t *b, data_t t, void *userdata) {
  struct machine *m = (struct machine *)userdata;
  point_t *position;
  data_t error;
  static data_t cur_time = 0;
  cur_time += m->cfg->tq;
  wait_next(m->cfg->tq * 1E9);

  switch(b->type) {
    case RAPID:
      position = &b->target;
      break;
    case LINE: {
      data_t l = block_lambda(b, t);
      point_t p = block_interpolate(b, l);
      position = &p;
      break;
    }
    case ARC_CW:
    case ARC_CCW:
      fprintf(stderr, "WARNING: arc interpolation is not yet implemented!\n");
      return STOP;
    default: {
      fprintf(stderr, "WARNING: unsupported block type: %s\n", b->line);
      return STOP;
    }
  }
  // communicate with the machine
  machine_go_to(m, position->x, position->y, position->z);
  machine_do_step(m, m->cfg->tq);
  error = machine_error(m);
  
  fprintf(stdout, "%3d %1d %7.3f %7.3f %7.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", b->n, b->type, cur_time, t, error, position->x, position->y, position->z, m->x->x, m->y->x, m->z->x);

  if (b->type == RAPID && error <= m->cfg->error) {
    return STOP;
  }

  return CONTINUE;
}

int main(int argc, char const *argv[]) {
  struct machine *m = machine_new("bin/config.lua");
  struct machine_config *cfg = m->cfg;
  program_t *p = program_new((char *)argv[1]);

  program_parse(p, cfg);
  program_print(p, stderr);

  machine_reset(m);
  machine_enable_viewer(m, "bin/MTViewer");
  sleep(1);

  fprintf(stderr, "Press SPACEBAR in the viewer to start\n");
  machine_wait_for_run(m);
  program_loop(p, time_loop, print_block_descr, m);

  program_free(p);
  machine_free(m);
  return 0;
}
