#include <stdio.h>
#include <stdlib.h>



float *f1(int n) {
  float *a = malloc(n*sizeof(float));
  int i;
  for (i = 0; i < n; i++) {
    a[i] = i;
  }
  return a;
}

void f2(float *a, int *n) {
  a = malloc(n*sizeof(float));
  int i;
  for (i = 0; i < *n; i++) {
    a[i] = i;
  }
  *n = 0;
}

void f3(float **a, int n) {
  *a = malloc(n*sizeof(float));
  int i;
  for (i = 0; i < n; i++) {
    (*a)[i] = i;
  }
}



int main(int argc, char const *argv[]) {
  float *a, *b, *c;

  a = f1(10);

  int n = 10;
  f2(b, &n);
  

  f3(&c, 10);

  free(a);
  free(c);

  return 0;
}
