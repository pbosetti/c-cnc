#include "block.h"
#include "ccnc.h"
#include <stdlib.h>

#ifndef LOOK_AHEAD_H
#define LOOK_AHEAD_H

typedef struct _look_ahead_strct look_ahead_t;

// if profile is NULL, then create a new profile and return a pointer to it;
// otherwise, simply resets the profile with the starting block
look_ahead_t *look_ahead_start(look_ahead_t *profile, block_t *start);

// stop the profile and do the calculations
void look_ahead_stop(look_ahead_t *profile, block_t *stop);

int look_ahead_is_open(look_ahead_t *lah);

// calculate again speed profiles of blocks in current segment
void look_ahead_update_blocks(look_ahead_t *lah);

void look_ahead_print(look_ahead_t *profile, FILE *dest);

void look_ahead_free(look_ahead_t *profile);

#endif