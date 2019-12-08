/* Linked list examples */
/* Nov. 18th 2019       */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// linked list structures:
// 1. the element type to be stored in the list
typedef struct {
  char *name;
  void *next, *prev;
} element_t;

// 2. the list itself
typedef struct {
  element_t *first, *last;
} list_t;


// create and return a new list with one element
list_t *list_new(char *name) {
  list_t *l = malloc(sizeof(list_t));
  element_t *e = malloc(sizeof(element_t));
  e->name = malloc(strlen(name) + 1);
  strncpy(e->name, name, strlen(name));
  l->first = e;
  l->last = e;
  return l;
}

// append one existing element at the end of the list
void list_append_element(list_t *list, element_t *e) {
  list->last->next = e;
  e->prev = list->last;
  list->last = e;
}

// create a new element and append it to the end of the list
void list_append(list_t *list, char *name) {
  element_t *e = malloc(sizeof(element_t));
  e->name = malloc(strlen(name) + 1);
  strncpy(e->name, name, strlen(name));
  list_append_element(list, e);
}

// insert a new element to the list immediately after 
// an element named "after"
void list_insert(list_t *list, char *name, char *after) {
  element_t *e;
  element_t *new = malloc(sizeof(element_t));
  new->name = malloc(strlen(name) + 1);
  strncpy(new->name, name, strlen(name));
  // why can't I simply do new->name = name; ???
  e = list->first;
  do {
    // for comparing two strings we use strcmp()
    if (strcmp(e->name, after) == 0)  {
      new->prev = e;
      new->next = e->next;
      ((element_t *)e->next)->prev = new;
      e->next = new;
      break;
    }
  } while ((e = e->next));
}

typedef enum {FIRST = 0, LAST} loop_order_t;
typedef int (*loop_fun)(element_t *e, void *userdata);
void list_loop(list_t *list, loop_fun f, loop_order_t order, void *userdata) {
  element_t *e;
  if (order == FIRST)
    e = list->first;
  else
    e = list->last;

  do {
    if (f(e, userdata) == 0) break;
    if (order == FIRST) e = e->next;
    else e = e->prev;
  } while (e);
}



//    _____      _ _ ____             _        
//   / ____|    | | |  _ \           | |       
//  | |     __ _| | | |_) | __ _  ___| | _____ 
//  | |    / _` | | |  _ < / _` |/ __| |/ / __|
//  | |___| (_| | | | |_) | (_| | (__|   <\__ \
//   \_____\__,_|_|_|____/ \__,_|\___|_|\_\___/

// print all elements
int print_all(element_t *e, void *userdata) {
  printf("%8s, %15p -> %15p -> %15p\n", e->name, e->prev, e, e->next);
  return 1;
}

// Print a range of elements, from start to start+span
struct range {size_t start, span;};

int print_range(element_t *e, void *userdata) {
  // static variables retain their values between calls
  // and must be inizialized. The initial value is the 
  // value that the static variable has the very first time
  // this function is executed
  static size_t i = 0;

  // the userdata must be casted to the expected type
  // before being used
  struct range *rng = (struct range *)userdata;

  // reset i and return 0 if we past the end of range
  // or if we reached the end of the list
  if (e == NULL || i >= rng->start + rng->span) {
    i = 0;
    return 0;
  } 

  // Only print if we past the start of range
  if (i >= rng->start) {
    printf("%8s, %15p -> %15p -> %15p\n", e->name, e->prev, e, e->next);
  }

  // increment the counter
  i++;
  return 1;
}



int main(int argc, char const *argv[]) {
  size_t i;
  element_t *e;
  // list of words
  char names[4][10] = {"one", "two", "three", "four"};
  // create a new list with the first word
  list_t *list = list_new(names[0]);

  // populating the list with contents of array names
  for (i = 1; i < 4; i++) {
    e = malloc(sizeof(element_t));
    e->name = names[i];
    list_append_element(list, e);
  }

  // print the content of the list in reverse order
  list_loop(list, print_all, LAST, NULL);
  printf("\n");

  // insert a new element with name "two.five" after "two"
  list_insert(list, "two.five", "two");
  // print the new content in normal order
  list_loop(list, print_all, FIRST, NULL);
  printf("\n");

  // Only print a slice of the array, from element 1
  // to element 1+2
  struct range rng = {1, 2};
  list_loop(list, print_range, FIRST, &rng);

  return 0;
}
