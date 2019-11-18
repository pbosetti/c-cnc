#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

int main(int argc, char const *argv[]) {
  uint16_t a;
  uint32_t la;
  void *p;
  float b;


  a = UINT16_MAX;
  la = a; // implicit up-casting
  a += 2;  // corresponds to a = a + 2
  la += 2;

  //example of explicit down-casting
  // assert(la < UINT16_MAX);
  printf("a = %d, la = %d, la = %d\n", a, la, (uint16_t)la); // type casting

  // void cast
  a = 10;
  b = 13.3;
  p = &a;
  printf("p = %d\n", *(uint16_t *)p);

  p = &b;
  printf("p = %f\n", *(float *)p);


  return 0;
}
