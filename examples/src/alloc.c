#include <stdio.h>
// malloc() is made available by stdlib.h
#include <stdlib.h>
#include <stdint.h>


float *dontwork(size_t n) {
  float a[n];
  size_t i;
  for (i = 0; i < n; i++) {
    a[i] = i;
  }
  return a;
}

void inout(size_t n, float *a) {
  size_t i;
  for (i = 0; i < n; i++) {
    a[i] = i;
  }
}

// Corresponding to CASE ONE on slides
void alloc_inout_dontwork(size_t n, float *a) {
  size_t i;
  a = malloc(sizeof(float)*n);
  for (i = 0; i < n; i++) {
    a[i] = i;
  }
}

// Corresponding to CASE TWO on slides
void alloc_inout(size_t n, float **a) {
  size_t i;
  *a = malloc(sizeof(float) * n);
  for (i = 0; i < n; i++) {
    (*a)[i] = i;  // read as: (content of a)[element i]
    // *a[i] = i; // this reads as: content of (a[element i])
  }
}


int main(int argc, char const *argv[]) {
  size_t n = 10;
  float *d1; // d1 will be an array of n elements
  float d2[n];
  float *d3 = NULL;
  float *d4 = NULL;

  d1 = dontwork(n);
  if (d1) 
    printf("d1[2]: %f\n", d1[2]);
  
  inout(n, d2);
  printf("d2[2]: %f\n", d2[2]);

  alloc_inout_dontwork(n, d3);
  if (d3)
    printf("d3[2]: %f\n", d3[2]);

  // &d4 is pronounced as "address of d4"
  // *d4 is pronounced as "content of d4"
  alloc_inout(n, &d4);
  if (d4)
    printf("d4[2]: %f\n", d4[2]);
  free(d4);
  return 0;
}
