//   _     _       _            _   _ _     _   
//  | |   (_)_ __ | | _____  __| | | (_)___| |_ 
//  | |   | | '_ \| |/ / _ \/ _` | | | / __| __|
//  | |___| | | | |   <  __/ (_| | | | \__ \ |_ 
//  |_____|_|_| |_|_|\_\___|\__,_| |_|_|___/\__|
//  
#include "llist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//    ____          _                         _                           _   
//   / ___|   _ ___| |_ ___  _ __ ___     ___| | ___ _ __ ___   ___ _ __ | |_ 
//  | |  | | | / __| __/ _ \| '_ ` _ \   / _ \ |/ _ \ '_ ` _ \ / _ \ '_ \| __|
//  | |__| |_| \__ \ || (_) | | | | | | |  __/ |  __/ | | | | |  __/ | | | |_ 
//   \____\__,_|___/\__\___/|_| |_| |_|  \___|_|\___|_| |_| |_|\___|_| |_|\__|

typedef struct element {
  char *id;
  struct element *next, *prev;
  // other custom fields
} element_t;

element_t *element_new(char *id) {
  element_t *e = malloc(sizeof(element_t));
  e->id = malloc(strlen(id) + 1);
  strcpy(e->id, id);
  return e;
}

void element_free(element_t *e) {
  free(e->id);
  free(e);
  e = NULL;
}

char *element_id(element_t *e) { return e->id; }

element_t *element_next(element_t *e) { return e->next; }

void element_set_next(element_t *e, element_t *next) { e->next = next; }

element_t *element_prev(element_t *e) { return e->prev; }

void element_set_prev(element_t *e, element_t *prev) { e->prev = prev; }

//    ____      _ _ _                _        
//   / ___|__ _| | | |__   __ _  ___| | _____ 
//  | |   / _` | | | '_ \ / _` |/ __| |/ / __|
//  | |__| (_| | | | |_) | (_| | (__|   <\__ \
//   \____\__,_|_|_|_.__/ \__,_|\___|_|\_\___/

void print_element(element_t *e, loop_order_t order, void *userdata) {
  // here we don't use order nor userdata
  printf("%10s: %15p -> %15p -> %15p\n", element_id(e), element_prev(e), e, element_next(e));
}

void print_element_with_num(element_t *e, loop_order_t order, void *userdata) {
  // before using a void*, we need to convert it (cast) into a proper type:
  size_t *i = (size_t *)userdata;
  printf("list[%ld] = %s\n", *i, element_id(e));
  if (order == ASC) (*i)++;
  else (*i)--;
}

//   __  __       _       
//  |  \/  | __ _(_)_ __  
//  | |\/| |/ _` | | '_ \
//  | |  | | (_| | | | | |
//  |_|  |_|\__,_|_|_| |_|

int main(int argc, char const *argv[]) {
  char names[4][10] = {"one", "two", "three", "four"};
  element_t *e;
  size_t i;
  // create a list:
  list_t *list = list_new("zero");

  for (i = 0; i < 4; i++) {
    list_append(list, names[i]);
  }

  // insert a new element in between "two" and "three"
  list_insert(list, "two.five", "two");

  // loop from first to last element:
  list_loop(list, print_element, ASC, NULL);

  // loop:
  i = 0;
  list_loop(list, print_element_with_num, ASC, &i);

  // again in descending order
  i = list->length - 1;
  list_loop(list, print_element_with_num, DESC, &i);

  // remove an element
  list_delete(list, "two.five");
  i = 0;
  list_loop(list, print_element, ASC, NULL);

  // free memory:
  list_free(list);

  return 0;
}
