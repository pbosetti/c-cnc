#include <stdint.h>

uint64_t factorial(int n) {
  if (n <= 1) {
    return 1;
  }
  else {
    return n * factorial(n - 1);
  }
}