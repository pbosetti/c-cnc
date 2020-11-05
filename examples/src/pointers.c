/*
  Lesson of 201105
*/
#include <stdio.h>
#include <stdlib.h>

// show as a function can have output arguments by using pointers.
// The memory pointed by the val_p pointer is changed within the function
// and the change survives the function exit.
// If the parameter were a plain value rather than a pointer, any change to 
// it would not survive, because parameters are PASSED BY COPY.
void increment(int *val_p) {
  *val_p += 1;
}

// Fill the array ary of length len with values val:
void array_fill(float *ary, int len, float val) {
  int i;
  for (i = 0; i < len; i++) {
    ary[i] = val;
  }
}

// print an array ary of length len
// alternative and completely equivalent signature:
//   void array_print(float *ary, int len)
void array_print(float ary[], int len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("a[%d] = %f\n", i, ary[i]);
  }
}

int main() {
  int a = 10;
  int *a_p; // declare a_p as a pointer to an int

  a_p = &a; // assign the memory location of a to pointer a_p

  // & is the operator that takes the location of a variable
  // * is the inverse of &: it gives the content of an address
  printf("a = %d; a_p address is %p; a_p value is %d\n", a, a_p, *a_p);

  // call the increment() function, which changes the content of the memory
  // location passed as argument:
  increment(a_p);
  printf("*a_p = %d\n", *a_p);
  printf("a    = %d\n", a);

  // equivalently, you can pass &a as argument, since &a is equal to a_p:
  increment(&a);
  printf("a    = %d\n", a);

  #define VEC_LEN 4
  float v[VEC_LEN] = {1, 2, 3, 4};
  int i;
  // show that an aray is actually a pointer to the first element of a 
  // list of contiguous memory locations:
  for (i = 0; i < VEC_LEN; i++) {
    // note that addresses are in offset of 4 bytes, the size of a float:
    printf("v[%d] address: %p\n", i, &(v[i]));
    // array elements can be accessed via indexing (i.e. []) or via pointer
    // arithmetics (i.e. v[2] is like v+2):
    printf("v[%d] value: %f\n", i, *(v+i));
  }
  
  // function calls with vectors as output parameters:
  int len = 6;
  float v2[len];
  array_fill(v2, len, 10.0);
  array_print(v2, len);

  // memory allocation on the heap
  float *large_v = malloc(1000 * sizeof(float));
  // now large_v can be accessed as a vector of 1000 floats
  array_fill(large_v, 1000, 10);
  // any memory allocated with malloc MUST BE FREED with free():
  free(large_v);

  return 0;
}