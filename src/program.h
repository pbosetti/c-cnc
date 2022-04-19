// C-CNC progam interface


#ifndef PROGRAM_H
#define PROGRAM_H

#include "defines.h"
#include "block.h"
#include "machine.h"


// data structures

typedef struct {
  char *filename; // file name
  FILE *file;     // file handle
  block_t *first, *last, *current;
  size_t n;  // total number of blocks
} program_t;


//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              
// create a new program from the given filename
program_t *program_new(const char *filename);

// deallocate
void program_free(program_t *program);

// parse the program
// return either EXIT_SUCCESS or EXIT_FAILURE
int program_parse(program_t *program, machine_t *cfg);

// linked-list navigation functions
void program_next(program_t *program);
void program_reset(program_t *program);

// print a program description
void program_print(program_t *program, FILE *output);

// GETTERS
char *program_filename(program_t *p);

block_t *program_curent(program_t *p);

block_t *program_first(program_t *p);

block_t *program_last(program_t *p);

size_t program_length(program_t *p);


#endif // end double inclusion guard