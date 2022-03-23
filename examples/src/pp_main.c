//   ____                                                        
//  |  _ \ _ __ ___ _ __  _ __ ___   ___ ___  ___ ___  ___  _ __ 
//  | |_) | '__/ _ \ '_ \| '__/ _ \ / __/ _ \/ __/ __|/ _ \| '__|
//  |  __/| | |  __/ |_) | | | (_) | (_|  __/\__ \__ \ (_) | |   
//  |_|   |_|  \___| .__/|_|  \___/ \___\___||___/___/\___/|_|   
//                 |_|                                           
// run this command: gcc -E examples/src/pp_main.c

// try to include a header twice and compare the output with and without
// the double inclusion guard:
#include "pp_include1.h"
#include "pp_include1.h"

int main(int argc, char const *argv[]) {
  double a = 2;
  printf("%s: %f\n", TEST, TWICE(a));
  return 0;
}
