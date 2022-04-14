//   ____  _            _    
//  | __ )| | ___   ___| | __
//  |  _ \| |/ _ \ / __| |/ /
//  | |_) | | (_) | (__|   < 
//  |____/|_|\___/ \___|_|\_\
//  Block class

#ifndef BLOCK_H
#define BLOCK_H

#include "defines.h"
#include "point.h"
#include "machine.h"

// Opaque structure representing a G-code block
typedef struct block block_t;

// Block types
typedef enum {
  RAPID = 0,
  LINE,
  ARC_CW,
  ARC_CCW,
  NO_MOTION
} block_type_t;


//   _____                 _   _                 
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___ 
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
                                              
block_t *block_new(const char *line, block_t *prev, machine_t *cfg);

void block_free(block_t *b);

// Parsing the G-code string. Returns an integer for success/failure
int block_parse(block_t *b);

// Evaluate the value of lambda at a certaint time
data_t block_lambda(const block_t *b, data_t time);

point_t *block_interpolate(block_t *b, data_t lambda);

void block_print(block_t *b, FILE *out);


// GETTERS

data_t block_length(const block_t *b);

data_t block_dtheta(const block_t *b);

point_t *block_center(const block_t *b);



#endif // BLOCK_H