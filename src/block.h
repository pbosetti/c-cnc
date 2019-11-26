

#ifndef BLOCK_H
#define BLOCK_H

#include "ccnc.h"

typedef enum {
  RAPID = 0,
  LINE,
  ARC_CW,
  ARC_CCW,
  NO_MOTION
} block_type_t;

typedef struct {
  data_t a, d;
  data_t f, l;
  data_t dt_1, dt_m, dt_2;
  data_t dt;
} block_profile_t;

// a block is a line of G-code
typedef struct block {
  char *line;
  block_type_t block_type;
  index_t n;
  index_t tool;
  point_t target;
  point_t delta;
  data_t length;
  data_t spindle;
  data_t feedrate;
  block_profile_t *prof;
  struct block *prev;
  struct block *next;
} block_t;

// Functions
block_t *block_new(char *line, block_t *prev);
void block_free(block_t *block);

index_t block_parse(block_t *block);

data_t block_lambda(block_t *block, data_t time);

point_t block_interpolate(block_t *block, data_t lambda);

void block_print(block_t *block, FILE *out);




#endif