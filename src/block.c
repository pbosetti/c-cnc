#include "block.h"
#include <ctype.h>

// Static, or private, functions
// are only accessible from within this file
static int block_set_field(block_t *b, char cmd, char *arg) {
  assert(b);
  switch (cmd) {
    case 'N':
      b->n = atol(arg);
      break;
    case 'G':
      b->type = atoi(arg);
      break;
    case 'X':
      point_x(&b->target, atof(arg));
      break;
    case 'Y':
      point_y(&b->target, atof(arg));
      break;
    case 'Z':
      point_z(&b->target, atof(arg));
      break;
    case 'F':
      b->feedrate = atof(arg);
      break;
    case 'S':
      b->spindle = atof(arg);
      break;
    case 'T':
      b->tool = atoi(arg);
      break;
    default:
      fprintf(stderr, "ERROR: Unexpected G-code command %c%s\n", cmd, arg);
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

static void block_compute(block_t *b) {

}

// public functions
block_t *block_new(char *line, block_t *prev) {
  assert(line);
  block_t *b = malloc(sizeof(block_t));
  assert(b);

  // memory setup
  if (prev) { // if there is a previous block
    memcpy(b, prev, sizeof(block_t));
    b->prev = prev;
    prev->next = b;
  }
  else { // this is the first block
    memset(b, 0, sizeof(block_t));
    b->prev = NULL; // redundant
  }

  // fields that must be calculated
  b->length = 0.0;
  b->target = point_new();
  b->delta = point_new();
  // allocate memory for referenced objects:
  b->prof = malloc(sizeof(block_profile_t));
  assert(b->prof);
  // copy line to b->line
  b->line = malloc(strlen(line) + 1);
  strcpy(b->line, line);
  return b;
}

void block_free(block_t *block) {
  assert(block);
  // only block line and prof if they are not NULL
  if (block->line) free(block->line);
  if (block->prof) free(block->prof);
  free(block);
}

int block_parse(block_t *block) {
  assert(block);
  char *line, *word, *to_free;
  to_free = line = strdup(block->line); // uses malloc internally

  while ((word  = strsep(&line, " ")) != NULL) {
    // parse each word
    // "x123.9" ->         'X',              "123.9"
    block_set_field(block, toupper(word[0]), word + 1);
  }
  free(to_free);
  
  // compute the fields to be calculated
  block_compute(block);
}

data_t block_lambda(block_t *block, data_t time) {

}

point_t  block_interpolate(block_t *block, data_t lambda) {

}

void block_print(block_t *block, FILE *out) {

}
