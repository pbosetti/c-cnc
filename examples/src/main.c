#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <stdint.h>

int main(int argc, char const *argv[]) {
  /* C types */

  // Type specifiers: char, int, float, and double
  // Type modifiers: signed, unsigned, short, and long

  // char: 8 bit number
  printf("char: [%d, %d, %d]\n", CHAR_MIN, CHAR_MAX, UCHAR_MAX);
  printf("short int: [%d, %d, %d]\n", SHRT_MIN, SHRT_MAX, USHRT_MAX);
  printf("int: [%d, %d, %u]\n", INT_MAX, INT_MIN, UINT_MAX);
  printf("long int: [%lu, %lu, %lu]\n", LONG_MIN, LONG_MAX, ULONG_MAX);
  printf("long long int: [%llu, %llu, %llu]\n", LLONG_MIN, LLONG_MAX, ULLONG_MAX);

  // floating points
  printf("float: [-%e, %e, %e]\n", FLT_MIN, FLT_MAX, FLT_EPSILON);
  printf("double: [-%e, %e, %e]\n", DBL_MIN, DBL_MAX, DBL_EPSILON);
  printf("long double: [-%Le, %Le, %Le]\n", LDBL_MIN, LDBL_MAX, LDBL_EPSILON);

  int16_t a = 0;
  uint32_t b = 10;
  return 0;
}



