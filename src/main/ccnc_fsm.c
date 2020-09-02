#include "../ccnc.h"
#include "../program.h"
#include <machine.h>
#include <limits.h>
#include <libgen.h> // for dirname()
#include <syslog.h>
#include "../fsm.h"

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
  state_data_t sd;

  // prepare paths to config and viewer
  strncpy(sd.this_dir, dirname((char *)argv[0]), PATH_MAX);
  sprintf(sd.config_path, "%s/config.lua", sd.this_dir);
  sprintf(sd.viewer_path, "%s/MTViewer", sd.this_dir);

  // check for proper number of arguments
  if (argc != 2) {
    fprintf(stderr, "I need the G-code file path!\n");
    return EXIT_FAILURE;
  }
  strncpy(sd.program_file, argv[1], PATH_MAX);

  state_t cur_state = STATE_INIT;
  openlog("CCNC", LOG_PID | LOG_PERROR, LOG_USER);
  syslog(LOG_INFO, "Starting SM");

  do {
    cur_state = run_state(cur_state, &sd);
  } while (cur_state != STATE_STOP);


  return 0;
}
