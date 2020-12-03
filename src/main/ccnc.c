// main executable for C-CNC
// from project root folder, compile with:
// cmake -Bbuild -H.
// make -Cbuild install

#include <machine.h>
#include "../ccnc.h"
#include "../program.h"
#include "../fsm.h"

int main(int argc, const char **argv) {
  state_data_t data;
  state_t cur_state = STATE_INIT;

  // copy command line parameters into data, so that 
  // they are available to state functions:
  data.argc = argc;
  data.argv = (char **)argv;

  // Main loop: it has to call the FSM manager run_state() and
  // to temporize the execution with the sampling time
  do {
    cur_state = run_state(cur_state, &data);
    wait_next(data.machine->cfg->tq * 1E9); // argument is in nanoseconds!
  } while (cur_state != STATE_STOP);
  // if a state function returns STATE_STOP, then the loop immediately stops
  // so we have to manually run the STATE_STOP function manually:
  run_state(cur_state, &data);

  return 0;
}