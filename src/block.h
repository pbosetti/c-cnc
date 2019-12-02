//    ____       ____ _   _  ____ 
//   / ___|     / ___| \ | |/ ___|
//  | |   _____| |   |  \| | |    
//  | |__|_____| |___| |\  | |___ 
//   \____|     \____|_| \_|\____|
// A parsed G-code block
#ifndef BLOCK_H
#define BLOCK_H

// include master header
#include "ccnc.h"
#include "point.h"
#include <machine.h>

// DATA STRUCTURES
// ===============

// valid and known block types
typedef enum {
  RAPID = 0,
  LINE,
  ARC_CW,
  ARC_CCW,
  NO_MOTION
} block_type_t;

// data for defining a velocity profile
typedef struct {
  data_t a, d;             // accelerations
  data_t f, l;             // feedrate and length
  data_t dt_1, dt_m, dt_2; // times
  data_t dt;               // time increment
} block_profile_t;

// a block is a line of G-code
typedef struct block {
  char *line;              // G-code line as a string
  block_type_t type;       // type of block
  index_t n;               // progressive number of block
  index_t tool;            // current tool
  data_t spindle;          // spindle speed
  data_t feedrate;         // tool feedrate
  point_t target;          // target position at the end of block
  data_t length;           // block length
  point_t delta;           // delta coordinates
  block_profile_t *prof;   // velocity profile data
  block_t *prev;           // next block (for linked list)
  block_t *next;           // previous block
  struct machine_config *config;
} block_t;

// FUNCTIONS
// =========

// create a new block with a given G-code string and a reference
// to the previous block. Data are copied from the previous block
block_t *block_new(char *line, block_t *prev);

// free memory allocated
void block_free(block_t *block);

// parse the lien field and update data from the G-code commands
// return EXIT_SUCCESS on success
int block_parse(block_t *block);

// evaluate the lambda function after t seconds from the beginning
data_t block_lambda(block_t *block, data_t time);

// return a point with the three projections at a given lambda value
point_t block_interpolate(block_t *block, data_t lambda);

// print a block description to out (either a file or a stream, like
// stdout or stderr)
void block_print(block_t *block, FILE *out);




#endif
