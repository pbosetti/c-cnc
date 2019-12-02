#include "../ccnc.h"
#include "../program.h"
#include <machine.h>

int main(int argc, char const *argv[]) {
  struct machine_config cfg;
  program_t *p = program_new((char *)argv[1]);
  cfg.A = 1;
  cfg.D = 1;
  cfg.tq = 0.005;
  cfg.zero[0] = 0;
  cfg.zero[1] = 0;
  cfg.zero[2] = 0;
  program_parse(p, &cfg);
  program_print(p, stdout);
  program_free(p);
  return 0;
}
