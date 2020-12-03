// main executable for C-CNC

#include <machine.h>
#include "../ccnc.h"
#include "../program.h"
#include "../fsm.h"

int main(int argc, const char **argv) {
  state_data_t data;
  state_t cur_state = STATE_INIT;

  data.argc = argc;
  data.argv = (char **)argv;

  do {
    cur_state = run_state(cur_state, &data);
    wait_next(data.machine->cfg->tq * 1E9);
  } while (cur_state != STATE_STOP);
  run_state(cur_state, &data);

  return 0;
}