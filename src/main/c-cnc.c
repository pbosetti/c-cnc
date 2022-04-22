#include "../defines.h"
#include "../machine.h"
#include "../program.h"
#include "../block.c"
#include "../point.c"

int main(int argc, char const *argv[]) {
  machine_t *machine = machine_new("settings.ini");
  if (!machine) {
    fprintf(stderr, "Error creating machine instance\n");
    exit(EXIT_FAILURE);
  }

  // TODO: create some blocks of G-code, and for a line or arc move
  // make a loop stepping in time with the given sampling time and
  // print out a table of time, lambda, feedrate, x, y, z

  machine_free(machine);
  return 0;
}
