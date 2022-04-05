//   _     _       _            _   _ _     _   
//  | |   (_)_ __ | | _____  __| | | (_)___| |_ 
//  | |   | | '_ \| |/ / _ \/ _` | | | / __| __|
//  | |___| | | | |   <  __/ (_| | | | \__ \ |_ 
//  |_____|_|_| |_|_|\_\___|\__,_| |_|_|___/\__|
//  Implemented as library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//   ___       _             __                
//  |_ _|_ __ | |_ ___ _ __ / _| __ _  ___ ___ 
//   | || '_ \| __/ _ \ '__| |_ / _` |/ __/ _ \
//   | || | | | ||  __/ |  |  _| (_| | (_|  __/
//  |___|_| |_|\__\___|_|  |_|  \__,_|\___\___|
//
// We assume that the list element are defined elsewhere.
// So we declare an OPAQUE STRUCT, i.e. an empty struct. C allows to do that, 
// as soon as the same struct is defined elsewhere (in the user's code).
typedef struct element element_t;
// Of course, without knowing the struct innards, we won't be able to 
// do thing as e->next, so we need to DECLARE accessor functions and let
// the user DEFINE those functions in h/er code:
char *element_id(element_t *e);
element_t *element_next(element_t *e);
void element_set_next(element_t *e, element_t *next);
element_t *element_prev(element_t *e);
void element_set_prev(element_t *e, element_t *prev);
// As well, the user has to define functions for creating and deleting
// an element:
element_t *element_new(char *id);
void element_free(element_t *e);
// The above part represents the INTERFACE of this class: if the user wants
// to make a linked list with an object s/he is building, then that object
// must comply with this interface, i.e. must provide a definition for the
// functions above defined.
// AND that object must be called element_t... 


//   _     _     _                   _      
//  | |   (_)___| |_    ___ ___   __| | ___ 
//  | |   | / __| __|  / __/ _ \ / _` |/ _ \
//  | |___| \__ \ |_  | (_| (_) | (_| |  __/
//  |_____|_|___/\__|  \___\___/ \__,_|\___|

// A structure representing the list itself
typedef struct {
  size_t length;
  element_t *first, *last;
} list_t;



//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/

// enum for the two possible direction in enumerating the list
typedef enum {ASC, DESC} loop_order_t;
                                              
// Create a new list, with one element newly created with "name"
list_t *list_new(char *name);

// apped an existing element e to the list
void list_append_element(list_t *list, element_t *e);

// create and append a new element with "name"
void list_append(list_t *list, char *name);

// create and insert a new element after the first element named after
void list_insert(list_t *list, char *name, char *after);

// delete an element with a given name
void list_delete(list_t *list, char *name);

// free memory
void list_free(list_t *list);

// prototype for callback function (named loop_fun_t)
typedef void (*loop_fun_t)(element_t *e, loop_order_t order, void *userdata);

// loop over the list and call the loop_fun_t f on each list element
// userdata can hold any pointer, and it is passed transparently to the callback
void list_loop(list_t *list, loop_fun_t f, loop_order_t order, void *userdata);