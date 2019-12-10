#include "../ccnc.h"
#include "../program.h"
#include <machine.h>
#include <limits.h>
#include <libgen.h> // for dirname()

//    ____      _ _ ____             _        
//   / ___|__ _| | | __ )  __ _  ___| | _____ 
//  | |   / _` | | |  _ \ / _` |/ __| |/ / __|
//  | |__| (_| | | | |_) | (_| | (__|   <\__ \
//   \____\__,_|_|_|____/ \__,_|\___|_|\_\___/

// block callback                                       
void print_block_descr(block_t *b, void*userdata) {
  // print block descrption on stderr
  block_print(b, stderr);
  // print column header for data table on stdout at the
  // beginning of each g-code block
  printf("#n type t_time b_time error x y z mx my mz\n");
}

// time loop callback
block_ctrl_t time_loop(block_t *b, data_t t, void *userdata) {
  struct machine *m = (struct machine *)userdata;
  point_t *position;
  data_t error;
  static data_t cur_time = 0; // static vars retain their value between calls

  // timing: keep track of passing time and wait until
  // system clock elapses the next multiple of tq
  cur_time += m->cfg->tq;
  wait_next(m->cfg->tq * 1E9);

  // deal with the block types
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
    // motion types that are not implemented (yet): give warning
    // and move to the next g-code block
    case ARC_CW:
    case ARC_CCW:
      fprintf(stderr, "WARNING: arc interpolation is not yet implemented!\n");
      return STOP;
    // catch all condition
    default: {
      fprintf(stderr, "WARNING: unsupported block type: %s\n", b->line);
      return STOP;
    }
  }

  // communicate with the machine
  machine_go_to(m, position->x, position->y, position->z); // axes setpoints
  machine_do_step(m, m->cfg->tq);                          // forward integrate axes dynamics
  // get the position error (distance of actual position from nominal position)
  error = machine_error(m);
  
  // print out values
  fprintf(stdout, "%3d %1d %7.3f %7.3f %7.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", b->n, b->type, cur_time, t, error, position->x, position->y, position->z, m->x->x, m->y->x, m->z->x);

  // a RAPID block stops when the error becomes smaller than a ive threshold
  if (b->type == RAPID && error <= m->cfg->error) {
    return STOP;
  }

  // by default, continue the loop
  return CONTINUE;
}

//   __  __       _          __                  _   _             
//  |  \/  | __ _(_)_ __    / _|_   _ _ __   ___| |_(_) ___  _ __  
//  | |\/| |/ _` | | '_ \  | |_| | | | '_ \ / __| __| |/ _ \| '_ \ 
//  | |  | | (_| | | | | | |  _| |_| | | | | (__| |_| | (_) | | | |
//  |_|  |_|\__,_|_|_| |_| |_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|

// The machine object needs the path of the configuration file and of the MTViewer executable.
// In C, relative paths when opening files are always relative to the current working dir.
// As such, if you hard-code the paths in the program, this program will only work
// when launched from the proper directory, which is no good.
// So the trick is to get the relative path of the current executable respect
// to the working directory with the function dirname(argv[0]), and then
// compose the paths to the needed files starting from this base path.  
int main(int argc, char const *argv[]) {
  struct machine *m;
  program_t *p;
  struct machine_config *cfg;
  char this_dir[PATH_MAX];    // PATH MAX is the maximum path length
  char config_path[PATH_MAX]; // for the current operating system
  char viewer_path[PATH_MAX];

  // prepare paths to config and viewer
  strncpy(this_dir, dirname((char *)argv[0]), PATH_MAX);
  sprintf(config_path, "%s/config.lua", this_dir);
  sprintf(viewer_path, "%s/MTViewer", this_dir);

  // check for proper number of arguments
  if (argc != 2) {
    fprintf(stderr, "I need the G-code file path!\n");
    return EXIT_FAILURE;
  }

  // initialize program and machine objects
  p = program_new((char *)argv[1]);
  m = machine_new(config_path);
  cfg = m->cfg;

  // deal with the program
  program_parse(p, cfg);
  fprintf(stderr, "Parsed program from %s\n", argv[1]);
  program_print(p, stderr);

  // Prepare for run: reset the machine and enable the viewer
  machine_reset(m);
  machine_enable_viewer(m, viewer_path);
  sleep(1);

  // set machine and viewer position accoriding to the config file
  machine_set_position(m, cfg->zero[0], cfg->zero[1], cfg->zero[2]);

  // wait for the user to hit spacebar in the viewer
  fprintf(stderr, "Press SPACEBAR in the viewer to start\n");
  machine_wait_for_run(m);

  // run loop
  program_loop(p, time_loop, print_block_descr, m);

  // release allocated memory
  program_free(p);
  machine_free(m);
  return 0;
}
