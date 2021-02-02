#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif
#include "../program.h"






int main (int argc, const char *argv[]) {
  char config_path[PATH_MAX];
  char this_path[PATH_MAX];
  struct machine_config *cfg;
  struct machine *machine;
  program_t *program;

  // machine_new() wants the path to the configuration file. It must be in the
  // same folder of the executable. The path to the current executable
  // is in argv[0], so we need to pick its directory part and append the
  // name of the config file (which is config.lua)
  strncpy(this_path, dirname((char *)argv[0]), PATH_MAX);
  sprintf(config_path, "%s/config.lua", this_path);

  // check that we have exactly one argument on the command line
  if (argc != 2) {
    fprintf(stderr, "Missing G-code file name!\n");
    return 1;
  }

  // create new machine simulator with the configuration in config_path
  machine = machine_new(config_path);
  // create a new program parser with the file passed to argv[1]
  program = program_new(argv[1]);
  // for brevity, use cfg as an alias to data->machine->cfg:
  cfg = machine->cfg;

  // stop if we have parsing errors
  if (program_parse(program, cfg) == EXIT_FAILURE) {
    fprintf(stderr, "Parsing error\n");
    return 2;
  }

  program_print(program, stderr);
  program_plot(program, "plot.pdf");

  block_t *b = program->first;
  point_t pos, prev_pos = point_new();
  point_xyz(&prev_pos, 0, 0, 0);
  data_t lambda;
  data_t t = 0, ct = 0, f = 0;
  printf("%3s %7s %7s %9s %9s %9s %5s %9s\n", "N", "T", "t", "X", "Y", "Z", "l", "f");
  do {
    t = 0;
    if (b->type == LINE || b->type == ARC_CW || b->type == ARC_CCW) {
      while ((lambda = block_lambda(b, t)) < 1) {
        pos = block_interpolate(b, lambda);
        f = point_dist(&prev_pos, &pos) / b->config->tq * 60;
        printf("%03d %7.3f %7.3f %9.3f %9.3f %9.3f %5.3f %9.3f\n", b->n, ct, t, pos.x, pos.y, pos.z, lambda, f);
        t += b->config->tq;
        ct += b->config->tq;
        memcpy(&prev_pos, &pos, sizeof(pos));
      }
    }
  } while ((b = b->next));

  return 0;
}
