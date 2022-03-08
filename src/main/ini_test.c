#include "../defines.h"
#include "../inic.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFLEN 1024

int main(int argc, char const *argv[]) {
  char *name = malloc(BUFLEN);
  int value;
  void *ini;
  if (argc == 2)
    ini = ini_init(argv[1]);
  else
    ini = ini_init("settings.ini");
  if (!ini) {
    fprintf(stderr, "Cannot open INI file, exiting.\n");
    return 1;
  }

  ini_get_char(ini, "test", "name", name, BUFLEN);
  ini_get_int(ini, "test", "value", &value);

  printf("Hello, world!\n");
  printf("%s version: %s\n", name, VERSION);
  printf("Parameter: %d\n", value);

  free(ini);
  free(name);
  return 0;
}
