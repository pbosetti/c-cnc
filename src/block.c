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

// Trapezoidal velocity profile
typedef struct {
  data_t a;                // acceleration
  data_t f, l;             // feedrate and length
  data_t dt_1, dt_m, dt_2; // trapezoid times
  data_t dt;               // total time
} block_profile_t;

// Block object structure
typedef struct block {
  char *line;            // G-code line
  block_type_t type;     // type of block
  size_t n;              // block number
  size_t tool;           // tool number
  data_t feedrate;       // feedrate
  data_t spindle;        // spindle rate
  point_t *target;       // destination point
  point_t *delta;        // distance vector w.r.t. previous point
  point_t *center;       // arc center (if it is an arc)
  data_t length;         // total length
  data_t i, j, r;        // center coordinates and radius (if it is an arc)
  data_t theta0, dtheta; // arc initial angle and arc angle
  data_t acc;            // actual acceleration
  machine_t *machine;    // machine configuration
  block_profile_t *prof; // velocity profile
  struct block *prev;    // next block (linked list)
  struct block *next;    // previous block
} block_t;


// STATIC FUNCTIONS (for internal use only)
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

void block_print(block_t *b, FILE *out) {}


// ALGORITHMS

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
// Definitions for the static functions declared above

// Parse a single G-code word (cmd+arg)
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




//   _____ _____ ____ _____   __  __       _       
//  |_   _| ____/ ___|_   _| |  \/  | __ _(_)_ __  
//    | | |  _| \___ \ | |   | |\/| |/ _` | | '_ \ 
//    | | | |___ ___) || |   | |  | | (_| | | | | |
//    |_| |_____|____/ |_|   |_|  |_|\__,_|_|_| |_|
//
#ifdef BLOCK_MAIN
int main() {
  return 0;
}
#endif