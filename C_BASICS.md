# Things to remember for C Programming

# Arrays

## Array declaration

An array can be declared in the stack memory or on the heap memory. 

* *on the stack*: the array or `n` elements is declared as: `double ary[n];`
* *on the heap*: the array is declared as: `double *ary;` then it must be assigned to a memory block allocated with proper size via a call to `malloc()`

In any case (stack or heap) in C an array is a pointer to its first element, so in both cases, `ary` points to the memory location containing the first value of the array. The square brackets operator is actualli asking for the content of a memory location with a given offset respect to the first element. So:

```c
double ary[3] = {1, 2, 3};
ary; // memory location containing 1
*ary; // content of memory location containing 1 (i.e., value of 1)
ary[2]; // content of the memory with offset 2 from ary (i.e., value of 3)
ary + 1; // memory location containing 2
*(ary + 1); // content ofthe previous, value of 2
```

The last two lines are an example of **pointer arithmetics**: incrementng or decrementing a pointer allows to read memory contents at a given offset from the original pointer.

## When to use stack or heap declared arrays

There are two reasons for selecting stack or heap declarations:

1. array size
2. array scope

**Array size**: the stack is property of the function where the array is declared, and it has a limited size (typically about 8 Mb per function), but it is a faster memory. Conversely, the heap capacity is only limited by the system total memory, but it is slower. So small arrays are beter declared on the stack, while large arrays have to be declared on the heap. Moreover, a stack array is *static*: you cannot change its size after declaration. Conversely, an heap array is *dynamic* and can be reallocated to a bigger (or smaller) size if needed.

**Array scope**: stack array only live in the stack, which is deleted upon function exit. So if you need to pass an array to other functions you have to declare it on the heap.

# Custom types and Compound types

## Custom types

The C language allows to define custom types as *alias* to existing types with the `typedef` keyword:

```c
typedef data_t double;
```

With such a definition I can now use the type `data_t` for variables representing real values. This is handy, because if in a later moment I want to use `float` types (in order to save some memory, for example), I can simply change the `typedef` (one single change) rather than cange all the declarations from `double` to `float` (possibly tens or hundreds of changes).

Typedef is also used for providing platform-independent types as those provided by the header `stdint.h`:

```c
#include <stdint.h>
...
uint16_t a; // unsigned 16 bit integer
int32_t b;  // signed 32 bit integer
```

Unlike native types (as `long int`) whose actual nit size depends on the architecture, those derived types always have the given, platform independent size.

## Compound types

A compound type is a structure: a collection of field of different types representing an object.

A structure is *defined* with the keyword `struct`: the general declaration is `struct <name> { type field1; type field2; ...}`. For example:

```c
struct my_struct {
  int a, b;
  double c;
  char *s;
  double *array;
}; // note the closing semicolon
```

An instance of this structure can be then declared and used as:

```c
struct my_struct ms1;
struct my_struct *ms2;
...
ms1.a = 1;
ms1.c = 2.3;
ms2->a = 2;
ms2->b = 9;
```

So, if a struct is declared as a normal variable its field are accessible as `struct_name.field_name` (dot notation). If the struct is declared as a pointer its fields are accessible as `struct_name->field_name` (arrow notation).

Now, `struct my_struct` represents a valid *type*. So it is possible to alias it to a shorter name by using typedef:

```c
typedef struct my_struct {
  //fields
} mystruct_t;
...
mystruct_t ms; // this is an instance of struct my_struct
```

When declaring structures as typedef, the structure name becomes optional, and you can omit it. The last block becomes:

```c
typedef struct { // no structure name here!
  //fields
} mystruct_t;
...
mystruct_t ms; // this is an instance of struct my_struct
```
2