//   _            _   
//  | |_ ___  ___| |_ 
//  | __/ _ \/ __| __|
//  | ||  __/\__ \ |_ 
//   \__\___||___/\__|
// simple test program

// local includes
#include "../defines.h"
#include "../inic.h"

// system includes
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// preprocessor macros and constants
#define BUFLEN 1024
#define INI_FILE "settings.ini"

int main(int argc, char const *argv[]) {
  // variables declarations
  int value = 0;
  void *ini = NULL;
  char *name = malloc(BUFLEN);

  // command line parsing
  if (argc == 2)
    ini = ini_init(argv[1]);
  else
    ini = ini_init(INI_FILE);

  // if the INI file cannot be read, exit immediately.
  if (!ini) {
    fprintf(stderr, "Cannot open INI file, exiting.\n");
    // exit code: not 0 => error
    return 1;
  }

  // read some valies from the INI file
  ini_get_char(ini, "test", "name", name, BUFLEN);
  ini_get_int(ini, "test", "value", &value);

  // print some messages
  printf("Hello, world!\n");
  printf("%s version: %s\n", name, VERSION);
  printf("Parameter: %d\n", value);

  // free memory from allocated resources
  free(ini);
  free(name);

  // exit code: 0 => all right
  return 0;
}
