//   _     _       _            _   _ _     _
//  | |   (_)_ __ | | _____  __| | | (_)___| |_
//  | |   | | '_ \| |/ / _ \/ _` | | | / __| __|
//  | |___| | | | |   <  __/ (_| | | | \__ \ |_
//  |_____|_|_| |_|_|\_\___|\__,_| |_|_|___/\__|
// Lesson of March 6th, 2022
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structure representing an object in the list
typedef struct element {
  char *id;
  struct element *next;
  struct element *prev;
} element_t;

// structure representing the list itself
typedef struct {
  element_t *first, *last;
  size_t length;
} list_t;

// The list is a chain of structures linked with twin pointers:
//         ┌───┬──────┐     ┌───┬──────┐     ┌───┬──────┐
// NULL ◄──┤ P │      │◄────┤ P │      │◄────┤ P │      │
//         ├───┘  ┌───┤     ├───┘  ┌───┤     ├───┘  ┌───┤
//         │      │ N ├────►│      │ N ├────►│      │ N ├──► NULL
//         └──────┴───┘     └──────┴───┘     └──────┴───┘


// Create a new list with one element in it
list_t *list_new(char *id) {
  // allocate memory for the list
  list_t *l = malloc(sizeof(list_t));
  // allocate memory for the element to be created
  element_t *e = malloc(sizeof(element_t));
  // if we are not sure that we are going to initialize all the fields
  // of e, then it is safer to set everything to zero
  memset(e, 0, sizeof(element_t));
  // create a dynamically allocated copy of id and copy that to e->id
  e->id = malloc(strlen(id) + 1);
  strncpy(e->id, id, strlen(id));
  // first element has nothing before and nothing after
  e->next = NULL;
  e->prev = NULL;
  // initialize list fields
  l->first = e;
  l->last = e;
  l->length = 1;
  return l;
}

// apending an existing element to the list
void list_append_element(list_t *list, element_t *e) {
  list->last->next = e; // the next element of current last becomes e
  e->prev = list->last; // the previus element of e becomes the previous last
  list->last = e;       // the new last element is e
  e->next = NULL;       // there is nothing after e
  list->length++;       // increase by 1 the list length
}

// creating and appending a new element
void list_append(list_t *list, char *id) {
  element_t *e = malloc(sizeof(element_t));
  e->id = malloc(strlen(id) + 1);
  strncpy(e->id, id, strlen(id));
  list_append_element(list, e);
}

// inserting an existing element after a given ID
//
//                             "after"
//         ┌───┬──────┐     ┌───┬──────┐       ┌───┬──────┐
// NULL ◄──┤ P │      │◄────┤ P │      │     ┌─┤ P │      │
//         ├───┘  ┌───┤     ├───┘  ┌───┤     │ ├───┘  ┌───┤
//         │      │ N ├────►│      │ N ├──┐  │ │      │ N ├──► NULL
//         └──────┴───┘     └────▲─┴───┘  │  │ └───▲──┴───┘
//                               │        │  │     │
//                               │        │  │     │
//                               │   ┌───┐▼──▼──┐  │
//                               └───┤ P │      │  │
//                       new element ├───┘  ┌───┤  │
//                    to be inserted │      │ N ├──┘
//                                   └──────┴───┘
//
void list_insert_element(list_t *list, element_t *new, char *after) {
  element_t *e;
  e = list->first;
  do {
    if (strcmp(e->id, after) == 0) {
      new->prev = e;
      new->next = e->next;
      e->next->prev = new;
      e->next = new;
      list->length++;
      break;
    }
  } while ((e = e->next));
}

// creating and inserting a new element after a given ID
void list_insert(list_t *list, char *id, char *after) {}

// delete an element with a given ID
void list_delete(list_t *list, char *id) {
  element_t *e;
  e = list->first;
  do {
    if (strcmp(e->id, id) == 0) { // id is found
      e->prev->next = e->next;
      e->next->prev = e->prev;
      free(e->id); // free memory of removed element
      free(e);
      list->length--;
      break;
    }
  } while ((e = e->next));
}

// Free all memory
void list_free(list_t *list) {
  element_t *e, *next;
  e = list->first;
  do {
    // before freeing element e, take note of its next element
    next = e->next;
    free(e->id);
    free(e);
  } while ((e = next));
  free(list);
}

// possible directions of traveling along the list
typedef enum {
  ASC, // from first to last, forward
  DESC // from last to first, backward
} loop_order_t;

// prototype of a function to be iterated over all elements in the list
// userdata is a pointer to anything: a value, an aray or a struct; this
// enables the user to implement the function with maximum flexibility
typedef void (*loop_fun_t)(element_t *e, loop_order_t o, void *userdata);

// loop over all elements in the list in a given order, calling fun over
// each element in turn
void list_loop(list_t *list, loop_fun_t fun, loop_order_t order,
               void *userdata) {
  element_t *e;
  // start from beginning or end, according to order
  if (order == ASC)
    e = list->first;
  else
    e = list->last;

  // loop over all elements
  do {
    // and call fun on each of them
    fun(e, order, userdata);
    // determine the next element according to order
    if (order == ASC)
      e = e->next;
    else
      e = e->prev;
  } while (e);
}





