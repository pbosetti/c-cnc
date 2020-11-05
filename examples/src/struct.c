#include <stdio.h>

typedef double data_t;

typedef struct {
  data_t x, y, z;
} point_t;

void point_print(point_t p) {
  printf("(%8.3f %f %f)\n", p.x, p.y, p.z);
  p.x = 2;
}

void array_print(data_t ary[], int len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("a[%d] = %f\n", i, ary[i]);
  }
}

#define ARY_LEN 6
int main() {
  point_t pt = {x:0, y:0, z:0};
  point_print(pt);
  pt.x = 123.456;
  point_print(pt);
  printf("pt.x = %f\n", pt.x);

  data_t ary[ARY_LEN] = {1, 2, 3, 4, 5, 6, 7};
  array_print(ary, ARY_LEN);
  return 0;
}