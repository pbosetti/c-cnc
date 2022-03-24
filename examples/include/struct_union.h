#include <stdio.h>
#include <stdlib.h>

#define ARY_LEN 6

typedef double data_t;

typedef struct {
  data_t x, y, z;
  // struct my_struct {
  //   float x,y,z;
  // };
} point_t;

typedef union {
  float x, y, z;
  // char str[20];
} my_union;

void point_print(point_t *a);

void print_array(data_t a[], int n);