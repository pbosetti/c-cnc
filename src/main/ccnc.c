#include "../ccnc.h"
#include "../program.h"
#include <machine.h>

// callbacks
void print_block_descr(block_t *b, void*userdata) {
  block_print(b, stdout);
}

block_ctrl_t time_loop(block_t *b, data_t t, void *userdata) {
  struct machine *m = (struct machine *)userdata;
  point_t *position;
  data_t error;
  
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

  if (b->type == RAPID && error <= m->cfg->error) {
    return STOP;
  }

  return CONTINUE;
}

int main(int argc, char const *argv[]) {
  struct machine_config cfg;
  struct machine *m;
  program_t *p = program_new((char *)argv[1]);
  cfg.A = 1;
  cfg.D = 1;
  cfg.tq = 0.005;
  cfg.zero[0] = 0;
  cfg.zero[1] = 0;
  cfg.zero[2] = 0;
  program_parse(p, &cfg);
  program_loop(p, time_loop, print_block_descr, m);

  program_free(p);
  return 0;
}
