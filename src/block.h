//   ____  _            _    
//  | __ )| | ___   ___| | __
//  |  _ \| |/ _ \ / __| |/ /
//  | |_) | | (_) | (__|   < 
//  |____/|_|\___/ \___|_|\_\
//
#ifndef BLOCK_H
#define BLOCK_H

// master include
#include "defines.h"
#include "machine.h"
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

typedef struct block block_t;


//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              
// create a new block, prev is the previous one
// (it can be NULL for the first block)
block_t *block_new(char *line, block_t *prev, machine_t *cfg);

// free memory for block
void block_free(block_t *block);

// parse the g-code line and fill the block fields
int block_parse(block_t *block);

// evaluate lambda function at a given time
data_t block_lambda(block_t *block, data_t time, data_t *v);

// interpolate motion on three axes at a given lambda value
point_t *block_interpolate(block_t *block, data_t lambda);

// print block description to channel out
void block_print(block_t *block, FILE *out);

data_t block_dt(block_t *block);

data_t block_dtheta(block_t *block);

point_t *block_center(block_t *block);





#endif // double inclusion guard