#include <stdio.h>
#include <stdlib.h>

#include "struct_union.h"

int main() {

  // point_t pt = {x:0, y:0, z:0}; // OLD VERSION
  // point_t pt = {.x=0, .y=0, .z=0, {.x=1, .y=2, .z=3}};
  point_t pt = {.x = 0, .y = 0, .z = 0};
  // point_t pt;
  // pt.x = 0;
  // pt.y = 0;
  // pt.z = 0;

  point_t *pt_ptr = &pt;
  pt.y = 20;
  pt_ptr->x = 10;

  point_print(pt_ptr);

  my_union another_pt;
  another_pt.x = 10;
  another_pt.y = 20;

  // {
  //   int sum = 10000;
  // }

  // printf("Sum is: %i\n", sum);

  printf("Size of union: %lu\n", sizeof(another_pt));
  printf("Size of pt:    %lu\n", sizeof(pt));

  printf("another_pt.x %f\n", another_pt.x);
  printf("another_pt.y %f\n", another_pt.y);

  data_t ary[ARY_LEN] = {10, 2};

  print_array(ary, ARY_LEN);

  return 0;
}

void point_print(point_t *a) { 
  printf("( %f %f %f )\n", a->x, a->y, a->z); 
}

void print_array(data_t a[], int n) {
  printf("[");
  // int i;
  for(int i=0; i<n; i++){
    printf(" %f ", a[i]);
    // a[i] = 10;
    // int my_cool_sum += a[i];
  } 
  printf("]\n");
}