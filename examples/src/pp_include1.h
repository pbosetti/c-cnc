// Try to rename PP_INCLUDE1_H and see what happens:
// the struct my_str is defined twice in the output, and this would
// raise an error in compilation

// double inclusion guard statement
#ifndef PP_INCLUDE1_H
#define PP_INCLUDE1_H

#include "pp_include2.h"

#define TEST "test"

typedef struct { int a; } my_str;

#endif // closing double-inclusion guard