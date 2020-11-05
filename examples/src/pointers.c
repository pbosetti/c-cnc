#include <stdio.h>
#include <stdlib.h>

void increment(int *val_p) {
  *val_p += 1;
}

void array_fill(float *ary, int len, float val) {
  int i;
  for (i = 0; i < len; i++) {
    ary[i] = val;
  }
}

void array_print(float ary[], int len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("a[%d] = %f\n", i, ary[i]);
  }
}

int main() {
  int a = 10;
  int *a_p;

  a_p = &a;

  printf("a = %d; a_p address is %p; a_p value is %d\n", a, a_p, *a_p);

  increment(a_p);
  printf("*a_p = %d\n", *a_p);
  printf("a    = %d\n", a);

  increment(&a);
  printf("a    = %d\n", a);

  float v[] = {1, 2, 3, 4};
  int i;
  for (i = 0; i < 4; i++) {
    printf("v[%d] address: %p\n", i, &(v[i]));
    printf("v[%d] value: %f\n", i, *(v+i));
  }
  
  int len = 6;
  float v2[len];
  array_fill(v2, len, 10.0);
  array_print(v2, len);

  // memory allocation on heap
  float *large_v = malloc(1000 * sizeof(float));
  // now large_v can be accessed as a vector of 1000 floats
  array_fill(large_v, 1000, 10);
  free(large_v);




  return 0;
}