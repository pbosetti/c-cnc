//   _     _       _            _   _ _     _   
//  | |   (_)_ __ | | _____  __| | | (_)___| |_ 
//  | |   | | '_ \| |/ / _ \/ _` | | | / __| __|
//  | |___| | | | |   <  __/ (_| | | | \__ \ |_ 
//  |_____|_|_| |_|_|\_\___|\__,_| |_|_|___/\__|
                                             
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


list_t *list_new(char *id) {
  list_t *l = malloc(sizeof(list_t));
  element_t *e = malloc(sizeof(element_t));
  memset(e, 0, sizeof(element_t));
  e->id = malloc(strlen(id) + 1);
  strncpy(e->id, id, strlen(id));
  e->next = NULL;
  e->prev = NULL;

  l->first = e;
  l->last = e;
  l->length = 1;
  return l;
}

// apending an existing element to the list
void list_append_element(list_t *list, element_t *e) {
  list->last->next = e; // the next element of current last becomes e
  e->prev = list->last; // the previus element of e becomes the previous last
  list->last = e; // the new last element is e
  e->next = NULL; // there is nothing after e
  list->length++; // increase by 1 the list length
}

// creating and appending a new element
void list_append(list_t *list, char *id) {
  element_t *e = malloc(sizeof(element_t));
  e->id = malloc(strlen(id) + 1);
  strncpy(e->id, id, strlen(id));
  list_append_element(list, e);
}

// inserting an existing element after a given ID
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
void list_insert(list_t *list, char *id, char *after) {

}

void list_delete(list_t *list, char *id) {
  element_t *e;
  e = list->first;
  do {
    if (strcmp(e->id, id) == 0) {
      e->prev->next = e->next;
      e->next->prev = e->prev;
      free(e->id);
      free(e);
      list->length--;
      break;
    }
  } while ((e = e->next));
}

void list_free(list_t *list) {
  element_t *e, *next;
  e = list->first;
  do {
    next = e->next;
    free(e->id);
    free(e);
  } while ((e = next));
  free(list);
}


typedef enum {ASC, DESC} loop_order_t;
typedef void (*loop_fun_t)(element_t *e, loop_order_t o, void *userdata);

void list_loop(list_t *list, loop_fun_t fun, loop_order_t order, void *userdata) {
  element_t *e;
  if (order == ASC) e =list->first;
  else e = list->last;

  do {
    fun(e, order, userdata);
    if (order == ASC) e = e->next;
    else e = e->prev;
  } while (e);
}

