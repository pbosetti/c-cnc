#include <stdio.h>
#include <stdlib.h>

void print_args(int argc, const char *argv[]) {
  printf("Number of args: #%i\n", argc);
  int i;
  for(i=0; i<argc; i++){
    printf("Args #%i is: %s\n", i, argv[i]);
  }
}

int main(int argc, const char *argv[]) {

  print_args(argc, argv);

  return 0;
}