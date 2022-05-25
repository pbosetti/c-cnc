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

//   _____                      
//  |_   _|   _ _ __   ___  ___ 
//    | || | | | '_ \ / _ \/ __|
//    | || |_| | |_) |  __/\__ \
//    |_| \__, | .__/ \___||___/
//        |___/|_|              

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

// LIFECYCLE ===================================================================

block_t *block_new(const char *line, block_t *prev, machine_t *cfg);
void block_free(block_t *b);
void block_print(block_t *b, FILE *out);

// ALGORITHMS ==================================================================

// Parsing the G-code string. Returns an integer for success/failure
int block_parse(block_t *b);

// Evaluate the value of lambda at a certaint time
// also return speed in the parameter v
data_t block_lambda(const block_t *b, data_t time, data_t *v);

// Interpolate lambda over three axes
point_t *block_interpolate(block_t *b, data_t lambda);


// GETTERS =====================================================================

data_t block_length(const block_t *b);
data_t block_dtheta(const block_t *b);
data_t block_dt(const block_t *b);
data_t block_r(const block_t *b);
block_type_t block_type(const block_t *b);
char *block_line(const block_t *b);
size_t block_n(const block_t *b);
point_t *block_center(const block_t *b);
block_t *block_next(const block_t *b);
point_t *block_target(const block_t *b);


#endif // BLOCK_H