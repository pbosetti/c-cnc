// this is the header for the Block object
#ifndef BLOCK_H
#define BLOCK_H

// master include
#include "ccnc.h"
// for toupper() function:
#include "point.h"

//   ____  _                   _       
//  / ___|| |_ _ __ _   _  ___| |_ ___ 
//  \___ \| __| '__| | | |/ __| __/ __|
//   ___) | |_| |  | |_| | (__| |_\__ \
//  |____/ \__|_|   \__,_|\___|\__|___/
                                    
typedef enum {
  RAPID = 0,
  LINE,
  ARC_CW,
  ARC_CCW,
  NO_MOTION
} block_type_t;

typedef struct {
  data_t a, d;             // aceleration and deceleration
  data_t f, l;             // feedrate and length
  data_t dt_1, dt_m, dt_2; // times
  data_t dt;               // timestep
} block_profile_t;

typedef struct block {
  char *line;            // G-code original line
  block_type_t type;     // Block type
  index_t n;             // Block number
  index_t tool;          // Tool number
  data_t feedrate;       // Feedrate (mm/min)
  data_t spindle;        // Spindle rate (rpm)
  point_t target;        // Target position at the end of the block
  data_t length;         // Block length
  point_t delta;         // Block projections
  block_profile_t *prof; // Velocity profile
  // Linked list pointers:
  struct block *next; // This allows b->next->line 
  struct block *prev;
  // machine configuration:
  struct machine_config *config;
} block_t;


//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              
// create a new block, prev is the previous one
// (it can be NULL for the first block)
block_t *block_new(char *line, block_t *prev, struct machine_config *cfg);

// free memory for block
void block_free(block_t *block);

// parse the g-code line and fill the block fields
int block_parse(block_t *block);

// evaluate lambda function at a given time
data_t block_lambda(block_t *block, data_t time);

// interpolate motion on three axes at a given lambda value
point_t  block_interpolate(block_t *block, data_t lambda);

// print block description to channel out
void block_print(block_t *block, FILE *out);







#endif // double inclusion guard