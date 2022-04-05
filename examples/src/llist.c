/* example of a linked list implementation */
/* Lesson of 201118                        */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist.h"

//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              
// Create a new list, with one element newly created with "name"
list_t *list_new(char *name) {
  list_t *l = malloc(sizeof(list_t)); // memory space for the list
  element_t *e = element_new(name); // memory for its first element
  element_set_prev(e, NULL); // these shall already be NULL, but different compilers 
  element_set_next(e, NULL); // can behave differently, so better safe than sorry
  l->first = e; // e is the only element, so it is both the first
  l->last =e;   // and the last
  l->length = 1;
  return l; // return the new list (as a pointer)
}

// apped an existing element e to the list
void list_append_element(list_t *list, element_t *e) {
  element_set_next(list->last, e); // point the next field of the last element to e
  element_set_prev(e, list->last); // previous item of e is the current last element
  list->last = e; // last element of the list is e
  element_set_next(e, NULL); // just to be sure
  list->length++;
}

// create and append a new element with "name"
void list_append(list_t *list, char *name) {
  // create the element
  element_t *e = element_new(name);
  // append it to the list
  list_append_element(list, e);
}

// create and insert a new element after the first element named after
void list_insert(list_t *list, char *name, char *after) {
  element_t *e; // this is gonna be the element named "after" 
  element_t *new = element_new(name);
  // loop and search for the first element with name=after
  e = list->first; // start looping form the beginning of the list
  do {
    if (strcmp(element_id(e), after) == 0) {
      element_set_prev(new, e);
      element_set_next(new, element_next(e));
      element_set_prev(element_next(e), new); // the cast is needed for e->next is of type void*
      element_set_next(e, new);
      break; // stop searching after the first item is found
    }
  } while ((e = element_next(e)));
  list->length++;
}

// delete an element with a given name
void list_delete(list_t *list, char *name) {
  element_t *e;
  e = list->first;
  do {
    if (strcmp(element_id(e), name) == 0) {
      // detach pointers from current element and 
      // attach them to the previous and next elements
      element_set_next(element_prev(e), element_next(e));
      element_set_prev(element_next(e), element_prev(e));
      // free memory taken from current element
      element_free(e);
      break;
    }
  } while ((e = element_next(e)));
  list->length--;
}

// free memory
void list_free(list_t *list) {
  element_t *e, *next;
  // first, free list contents
  e = list->first; // start looping form the beginning of the list
  do {
    next = element_next(e); // temporary storage, for we are going to free(e)
    element_free(e);
  } while ((e = next));
  // finally, free the list itself
  free(list);
}

// loop over the list and call the loop_fun_t f on each list element
// userdata can hold any pointer, and it is passed transparently to the callback
void list_loop(list_t *list, loop_fun_t f, loop_order_t order, void *userdata) {
  element_t *e;
  if (order == ASC) e = list->first;
  else e = list->last;

  do {
    // call the function f
    f(e, order, userdata);
    if (order == ASC) e = element_next(e);
    else e = element_prev(e);
  } while (e);
}

