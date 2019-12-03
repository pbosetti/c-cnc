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

typedef enum {
  STOP = 0,
  CONTINUE
} block_ctrl_t;

typedef block_ctrl_t (*timeloop_t)(block_t *b, data_t time, void *userdata);
typedef void (*blockloop_t)(block_t *b, void *userdata);

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
// timestep is a callback function to be exec. at each timestep (NOT NULL!)
// new_block is a callback to be exec at every block, can be NULL
// userdata is passed to timestep and new_block
void program_loop(program_t *program,
        timeloop_t timestep,
        blockloop_t new_block, void *userdata);


#endif
