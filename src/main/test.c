#include "../defines.h"
#include "../inic.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  char *name = malloc(1024);
  int value;
  void *ini = ini_init("settings.ini");

  ini_get_char(ini, "test", "name", name);
  ini_get_int(ini, "test", "value", &value);

  printf("Hello, world!\n");
  printf("%s version: %s\n", name, VERSION);
  printf("Parameter: %d\n", value);

  free(ini);
  free(name);
  return 0;
}
