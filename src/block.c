//   ____  _            _
//  | __ )| | ___   ___| | __
//  |  _ \| |/ _ \ / __| |/ /
//  | |_) | | (_) | (__|   <
//  |____/|_|\___/ \___|_|\_\

#include "block.h"
#include <string.h>

//   ____            _                 _   _
//  |  _ \  ___  ___| | __ _ _ __ __ _| |_(_) ___  _ __  ___
//  | | | |/ _ \/ __| |/ _` | '__/ _` | __| |/ _ \| '_ \/ __|
//  | |_| |  __/ (__| | (_| | | | (_| | |_| | (_) | | | \__ \
//  |____/ \___|\___|_|\__,_|_|  \__,_|\__|_|\___/|_| |_|___/

typedef struct {
  data_t a;
  data_t f, l;
  data_t dt_1, dt_m, dt_2;
  data_t dt;
} block_profile_t;

typedef struct block {
  char *line;
  block_type_t type;
  size_t n;
  size_t tool;
  data_t feedrate;
  data_t spindle;
  point_t *target;
  point_t *delta;
  point_t *center;
  data_t length;
  data_t i, j, r;
  data_t theta0, dtheta;
  data_t acc;
  machine_t *machine;
  block_profile_t *prof;
  struct block *prev;
  struct block *next;
} block_t;

static int block_set_fields(block_t *b, char cmd, char *arg);


//   _____                 _   _
//  |  ___|   _ _ __   ___| |_(_) ___  _ __  ___
//  | |_ | | | | '_ \ / __| __| |/ _ \| '_ \/ __|
//  |  _|| |_| | | | | (__| |_| | (_) | | | \__ \
//  |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/

// Lifecycle
block_t *block_new(const char *line, block_t *prev, machine_t *cfg) {
  assert(line && cfg); // prev is NULL if this is the first block
  block_t *b = (block_t *)calloc(1, sizeof(block_t));
  if (!b) {
    perror("Error creating a block");
    exit(EXIT_FAILURE);
  }

  if (prev) { // copy the memory from the previous block
    memcpy(b, prev, sizeof(block_t));
    b->prev = prev;
    prev->next = b;
  } else { // this is the first block
    // nothing to do
  }

  // non-modal g-code parameters: I, J, R
  b->i = b->j = b->r = 0.0;

  // fields to be calculated
  b->length = 0.0;
  b->target = point_new();
  b->delta = point_new();
  b->center = point_new();
  // allocate memory for profile struct
  b->prof = (block_profile_t *)calloc(1, sizeof(block_profile_t));
  if (!b->prof) {
    perror("Error creating a profile structure");
    exit(EXIT_FAILURE);
  }
  b->machine = cfg;
  b->type = NO_MOTION;
  b->acc = machine_A(b->machine);
  b->line = (char *)malloc(sizeof(line));
  strcpy(b->line, line);

  return b;
}

void block_free(block_t *b) {
  assert(b);
  if (b->line)
    free(b->line);
  if (b->prof)
    free(b->prof);
  point_free(b->target);
  point_free(b->center);
  point_free(b->delta);
  free(b);
  b = NULL;
}

// Parsing the G-code string. Returns an integer for success/failure
int block_parse(block_t *b) {
  assert(b);
  char *word, *line, *tofree;

  tofree = line = strdup(b->line);
  if (!line) {
    perror("Error copying line");
    exit(EXIT_FAILURE);
  }
  // Tokenizing loop
  while ((word = strsep(&line, " ")) != NULL) {
    // word[0] is the command
    // word+1 is the pointer to the argument as a string
    block_set_fields(b, word[0], word + 1);
  }
  free(tofree);
}

// Evaluate the value of lambda at a certaint time
data_t block_lambda(const block_t *b, data_t time) {}

point_t *block_interpolate(block_t *b, data_t lambda) {}

void block_print(block_t *b, FILE *out) {}

// GETTERS

data_t block_length(const block_t *b) {
  assert(b);
  return b->length;
}

data_t block_dtheta(const block_t *b) {
  assert(b);
  return b->dtheta;
}

point_t *block_center(const block_t *b) {
  assert(b);
  return b->center;
}


//   ____  _        _   _         __                  
//  / ___|| |_ __ _| |_(_) ___   / _|_   _ _ __   ___ 
//  \___ \| __/ _` | __| |/ __| | |_| | | | '_ \ / __|
//   ___) | || (_| | |_| | (__  |  _| |_| | | | | (__ 
//  |____/ \__\__,_|\__|_|\___| |_|  \__,_|_| |_|\___|
                                                   
static int block_set_fields(block_t *b, char cmd, char *arg) {
  assert(b && arg);
  switch (cmd)
  {
  case 'N':
    b->n = atol(arg);
    break;
  case 'G':
    b->type = (block_type_t)atoi(arg);
    break;
  case 'X':
    point_set_x(b->target, atof(arg));
    break;
  case 'Y':
    point_set_y(b->target, atof(arg));
    break;
  case 'Z':
    point_set_z(b->target, atof(arg));
    break;
  case 'I': 
    b->i = atof(arg);
    break;
  case 'J':
    b->j = atof(arg);
    break;
  case 'R':
    b->r = atof(arg);
    break;
  case 'F':
    b->feedrate = atof(arg);
    break;
  case 'S':
    b->spindle = atof(arg);
    break; 
  case 'T':
    b->tool = atol(arg);   
    break;
    
  default:
    break;
  }
}



#ifdef BLOCK_MAIN
int main() {}
#endif