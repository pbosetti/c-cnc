//    ____       ____ _   _  ____ 
//   / ___|     / ___| \ | |/ ___|
//  | |   _____| |   |  \| | |    
//  | |__|_____| |___| |\  | |___ 
//   \____|     \____|_| \_|\____|
// A G-code program: read from a file and build a linked list of blocks
#ifndef PROGRAM_H
#define PROGRAM_H

// include master header
#include "ccnc.h"
#include "block.h"
#include <machine.h>


// DATA STRUCTURES
// ===============

typedef struct {
  char *filename;  // path to the g-code file
  FILE *file;      // file handle
  block_t *first, *last; // linked list extremes
  index_t n;       // total number of blocks in file
} program_t;


// FUNCTIONS
// =========

// create a new program from the given filename
program_t *program_new(char * filename);

// deallocate program and its contents
void program_free(program_t *program);

// parse the program
// result is either EXIT_FAILURE or EXIT_SUCCESS
int program_parse(program_t *program, struct machine_config *cfg);

// print a description of all blocks in program
void program_print(program_t *program, FILE *out);

// loop over the whole program
void program_loop(program_t *program);


#endif
