#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
  char *name;
  void *next, *prev;
} element_t;

typedef struct {
  element_t *first, *last;
} list_t;


list_t *list_new(char *name) {
  list_t *l = malloc(sizeof(list_t));
  element_t *e = malloc(sizeof(element_t));
  e->name = malloc(strlen(name) + 1);
  strncpy(e->name, name, strlen(name));
  l->first = e;
  l->last = e;
  return l;
}

void list_append_element(list_t *list, element_t *e) {
  list->last->next = e;
  e->prev = list->last;
  list->last = e;
}

void list_append(list_t *list, char *name) {
  element_t *e = malloc(sizeof(element_t));
  e->name = malloc(strlen(name) + 1);
  strncpy(e->name, name, strlen(name));
  list_append_element(list, e);
}

void list_insert(list_t *list, char *name, char *after) {
  element_t *e;
  element_t *new = malloc(sizeof(element_t));
  new->name = malloc(strlen(name) + 1);
  strncpy(new->name, name, strlen(name));
  // why can't I simply do new->name = name; ???
  e = list->first;
  do {
    if (strcmp(e->name, after) == 0)  {
      new->prev = e;
      new->next = e->next;
      ((element_t *)e->next)->prev = new;
      e->next = new;
      break;
    }
  } while (e = e->next);
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

  e = list->last;
  do {
    printf("%8s, %15p -> %15p -> %15p\n", e->name, e->prev, e, e->next);
  } while (e = e->prev);

  list_insert(list, "two.five", "two");
  e = list->first;
  do {
    printf("%8s, %15p -> %15p -> %15p\n", e->name, e->prev, e, e->next);
  } while (e = e->next);

  return 0;
}
