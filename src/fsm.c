/******************************************************************************
Finite State Machine
Project: fsm.dot
Description: C_CNC

Generated by gv_fsm ruby gem, see https://rubygems.org/gems/gv_fsm
gv_fsm version 0.2.8
Generation date: 2020-11-27 13:42:33 +0100
Generated from: fsm.dot
The finite state machine has:
  7 states
  1 transition functions
******************************************************************************/

#include "fsm.h"
#include <limits.h>
#include <libgen.h>


static inline void print_status(block_t *b, state_data_t *data, data_t error, point_t *pos) {
  struct machine *m = data->machine;
  printf("%3d %1d %7.3f %7.3f %7.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", 
    b->n, b->type, data->total_time, data->block_time, error,
    pos->x, pos->y, pos->z,
    m->x->x, m->y->x, m->z->x
  );
}

// SEARCH FOR Your Code Here FOR CODE INSERTION POINTS!

// GLOBALS
// State human-readable names
const char *state_names[] = {"init", "idle", "load_block", "stop", "rapid_move", "linear_move", "no_move"};

// List of state functions
state_func_t *const state_table[NUM_STATES] = {
  do_init,        // in state init
  do_idle,        // in state idle
  do_load_block,  // in state load_block
  do_stop,        // in state stop
  do_rapid_move,  // in state rapid_move
  do_linear_move, // in state linear_move
  do_no_move,     // in state no_move
};

// Table of transition functions
transition_func_t *const transition_table[NUM_STATES][NUM_STATES] = {
  /* states:         init , idle , load_block, stop , rapid_move, linear_move, no_move */
  /* init        */ {NULL , reset, NULL , NULL , NULL , NULL , NULL }, 
  /* idle        */ {NULL , NULL , NULL , NULL , NULL , NULL , NULL }, 
  /* load_block  */ {NULL , reset, NULL , NULL , NULL , NULL , NULL }, 
  /* stop        */ {NULL , NULL , NULL , NULL , NULL , NULL , NULL }, 
  /* rapid_move  */ {NULL , reset, NULL , NULL , NULL , NULL , NULL }, 
  /* linear_move */ {NULL , reset, NULL , NULL , NULL , NULL , NULL }, 
  /* no_move     */ {NULL , NULL , NULL , NULL , NULL , NULL , NULL }, 
};

//  ____  _        _       
// / ___|| |_ __ _| |_ ___ 
// \___ \| __/ _` | __/ _ \
//  ___) | || (_| | ||  __/
// |____/ \__\__,_|\__\___|
//                         
//   __                  _   _                 
//  / _|_   _ _ __   ___| |_(_) ___  _ __  ___ 
// | |_| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
// |  _| |_| | | | | (__| |_| | (_) | | | \__ \
// |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
//                                             

// Function to be executed in state init
// valid return states: STATE_IDLE
state_t do_init(state_data_t *data) {
  state_t next_state = STATE_IDLE;

  /* Your Code Here */
  char this_dir[PATH_MAX];
  char config_path[PATH_MAX];
  strncpy(this_dir, dirname(data->argv[0]), PATH_MAX);
  sprintf(config_path, "%s/config.lua", this_dir);
  struct machine_config *cfg;

  if (data->argc != 2) {
    fprintf(stderr, "Missing G-code program on command line\n");
    return STATE_STOP;
  }

  data->program = program_new(data->argv[1]);
  data->machine = machine_new(config_path);
  cfg = data->machine->cfg;

  if (program_parse(data->program, cfg) == EXIT_FAILURE)
    return STATE_STOP;
  
  program_print(data->program, stderr);

  machine_reset(data->machine);
  machine_set_position(data->machine, cfg->zero[0], cfg->zero[1], cfg->zero[2]);
  
  switch (next_state) {
    case STATE_IDLE:
      break;
    default:
      next_state = NO_CHANGE;
  }
  return next_state;
}


// Function to be executed in state idle
// valid return states: NO_CHANGE, STATE_IDLE, STATE_LOAD_BLOCK, STATE_STOP
state_t do_idle(state_data_t *data) {
  state_t next_state = NO_CHANGE;

  /* Your Code Here */
  data->total_time = 0.0;
  next_state = STATE_LOAD_BLOCK;
  
  switch (next_state) {
    case NO_CHANGE:
    case STATE_IDLE:
    case STATE_LOAD_BLOCK:
    case STATE_STOP:
      break;
    default:
      next_state = NO_CHANGE;
  }
  return next_state;
}


// Function to be executed in state load_block
// valid return states: STATE_IDLE, STATE_RAPID_MOVE, STATE_LINEAR_MOVE, STATE_NO_MOVE, STATE_STOP
state_t do_load_block(state_data_t *data) {
  state_t next_state = STATE_NO_MOVE;

  /* Your Code Here */
  block_t *b;
  program_next(data->program);
  b = data->program->current;
  if (b == NULL)
    next_state = STATE_STOP;
  else {
    fprintf(stderr, "> %s\n", b->line);
    switch (b->type)
    {
    case RAPID:
      next_state = STATE_RAPID_MOVE;
      break;
    case LINE:
      next_state = STATE_LINEAR_MOVE;
      break;
    case ARC_CW:
    case ARC_CCW:
      fprintf(stderr, "Arc interpolation not supported, exiting\n");
      next_state = STATE_STOP;
    case NO_MOTION:
      next_state = STATE_NO_MOVE;
      break;
    default:
      fprintf(stderr, "Unsupported block type G%02d, exiting\n", b->type);
      next_state = STATE_STOP;
      break;
    }
    data->block_time = 0.0;
  }
  
  switch (next_state) {
    case STATE_IDLE:
    case STATE_RAPID_MOVE:
    case STATE_LINEAR_MOVE:
    case STATE_NO_MOVE:
    case STATE_STOP:
      break;
    default:
      next_state = NO_CHANGE;
  }
  return next_state;
}


// Function to be executed in state stop
// valid return states: NO_CHANGE
state_t do_stop(state_data_t *data) {
  state_t next_state = NO_CHANGE;

  /* Your Code Here */
  fprintf(stderr, "Regular exit.\n");
  program_free(data->program);
  machine_free(data->machine);
  
  switch (next_state) {
    case NO_CHANGE:
      break;
    default:
      next_state = NO_CHANGE;
  }
  return next_state;
}


// Function to be executed in state rapid_move
// valid return states: NO_CHANGE, STATE_IDLE, STATE_LOAD_BLOCK, STATE_RAPID_MOVE, STATE_STOP
state_t do_rapid_move(state_data_t *data) {
  state_t next_state = NO_CHANGE;

  /* Your Code Here */
  data_t error;
  struct machine *m = data->machine;
  block_t *b = data->program->current;
  point_t *pos = &b->target;

  machine_go_to(m, pos->x, pos->y, pos->z);
  machine_do_step(m, m->cfg->tq);
  error = machine_error(m);

  data->block_time += m->cfg->tq;
  data->total_time += m->cfg->tq;
  if (error <= m->cfg->error) next_state = STATE_LOAD_BLOCK;

  print_status(b, data, error, pos);
  
  switch (next_state) {
    case NO_CHANGE:
    case STATE_IDLE:
    case STATE_LOAD_BLOCK:
    case STATE_RAPID_MOVE:
    case STATE_STOP:
      break;
    default:
      next_state = NO_CHANGE;
  }
  return next_state;
}


// Function to be executed in state linear_move
// valid return states: NO_CHANGE, STATE_IDLE, STATE_LOAD_BLOCK, STATE_LINEAR_MOVE, STATE_STOP
state_t do_linear_move(state_data_t *data) {
  state_t next_state = NO_CHANGE;

  /* Your Code Here */
  data_t error;
  struct machine *m = data->machine;
  block_t *b = data->program->current;
  point_t pos;
  data_t lambda;

  lambda = block_lambda(b, data->block_time);
  pos = block_interpolate(b, lambda);

  machine_go_to(m, pos.x, pos.y, pos.z);
  machine_do_step(m, m->cfg->tq);
  error = machine_error(m);

  data->block_time += m->cfg->tq;
  data->total_time += m->cfg->tq;
  if (data->block_time >= b->prof->dt)
    next_state = STATE_LOAD_BLOCK;

  print_status(b, data, error, &pos);

  switch (next_state) {
    case NO_CHANGE:
    case STATE_IDLE:
    case STATE_LOAD_BLOCK:
    case STATE_LINEAR_MOVE:
    case STATE_STOP:
      break;
    default:
      next_state = NO_CHANGE;
  }
  return next_state;
}


// Function to be executed in state no_move
// valid return states: STATE_LOAD_BLOCK
state_t do_no_move(state_data_t *data) {
  state_t next_state = STATE_LOAD_BLOCK;

  /* Your Code Here */
  fprintf(stderr, "No move in line %s\n", data->program->current->line);
  
  switch (next_state) {
    case STATE_LOAD_BLOCK:
      break;
    default:
      next_state = NO_CHANGE;
  }
  return next_state;
}


//  _____                    _ _   _              
// |_   _| __ __ _ _ __  ___(_) |_(_) ___  _ __   
//   | || '__/ _` | '_ \/ __| | __| |/ _ \| '_ \
//   | || | | (_| | | | \__ \ | |_| | (_) | | | | 
//   |_||_|  \__,_|_| |_|___/_|\__|_|\___/|_| |_| 
//                                                
//   __                  _   _                 
//  / _|_   _ _ __   ___| |_(_) ___  _ __  ___ 
// | |_| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
// |  _| |_| | | | | (__| |_| | (_) | | | \__ \
// |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
//    
                                         
// This function is called in 4 transitions:
// 1. from init to idle
// 2. from load_block to idle
// 3. from rapid_move to idle
// 4. from linear_move to idle
void reset(state_data_t *data) {
  /* Your Code Here */
  program_reset(data->program);
}


//  ____  _        _        
// / ___|| |_ __ _| |_ ___  
// \___ \| __/ _` | __/ _ \
//  ___) | || (_| | ||  __/ 
// |____/ \__\__,_|\__\___| 
//                          
//                                              
//  _ __ ___   __ _ _ __   __ _  __ _  ___ _ __ 
// | '_ ` _ \ / _` | '_ \ / _` |/ _` |/ _ \ '__|
// | | | | | | (_| | | | | (_| | (_| |  __/ |   
// |_| |_| |_|\__,_|_| |_|\__,_|\__, |\___|_|   
//                              |___/           

state_t run_state(state_t cur_state, state_data_t *data) {
  state_t new_state = state_table[cur_state](data);
  if (new_state == NO_CHANGE) new_state = cur_state;
  transition_func_t *transition = transition_table[cur_state][new_state];
  if (transition)
    transition(data);
  return new_state == NO_CHANGE ? cur_state : new_state;
};

#ifdef TEST_MAIN
#include <unistd.h>
int main(int argc, const char **argv) {
  state_data_t data;
  state_t cur_state = STATE_INIT;

  data.argc = argc;
  data.argv = argv;

  do {
    cur_state = run_state(cur_state, &data);
    // sleep(1);
  } while (cur_state != STATE_STOP);
  run_state(cur_state, &data);
  return 0;
}
#endif