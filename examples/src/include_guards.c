#include "include_guards1.h" 
#include "include_guards1.h" 

void print_args(int argc, const char *argv[]) {
  printf("Number of args: #%i\n", argc);
  int i;
  for(i=0; i<argc; i++){
    printf("Args #%i is: %s\n", i, argv[i]);
  }
}

int main(int argc, const char *argv[]) {

  print_args(argc, argv);

  if(argc == 1) {
    printf("I don't have enough arguments!\n");
    return 0;
  }

  double a = atof(argv[1]);

  printf("Symbol test is: %s\n", TEST);
  printf("Test TWICE(%f): %f\n", a, TWICE(a));

  return 0;
}