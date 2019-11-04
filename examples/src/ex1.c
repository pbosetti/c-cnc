#include <stdio.h>
#include <stdint.h>

// include of local headers need qiotes (") rather than angle brakets(<>)
// argument of include must be a valid path (relative to the current file)
#include "mylib.h"

int main(int argc, char const *argv[]) {
  uint16_t i;

  // assignments: 
  // i = i + 1;
  // i += 1;
  // i++; 
  // are all equal

  // loops
  for(i = 0; i <= 10; i += 2) { // i++ is a shortcut for i = i + 1
    if (i % 4 == 0) {
      printf("i = %d (divisible by 4)\n", i);
    }
    else {
      printf("i = %d\n", i);
    }
  }

  // variables scope
  // any variable is valid only within its enclosing curly braces
  {
    int16_t a = 25;
  }

  // functions

  printf("5^2 = %f\n", square(5.0));

  // array
  int my_ary[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  for (i = 0; i <= 10; i++) {
    printf("a[%d] = %d\n", i, my_ary[i]); // garbage at position 10
  }

  // strings
  char name[10] = "Tommaso";
  printf("name = %s\n", name);
  name[3] = 0; // '\0';
  printf("name = %s\n", name);
  for (i = 0; i <= 9; i++) {
    printf("name[%d] = %c\n", i, name[i]);
  }

  // command line arguments
  printf("I got %d arguments\n", argc);
  for (i = 0; i < argc; i++) {
    printf("arg[%d] = %s\n", i, argv[i]);
  }
  

  

  return 0;
}
