#include "block.h"

void block_set_field(block_t *b, char cmd, char *arg) {

}


block_t *block_new(char *line, block_t *prev) {
  // line cannot be NULL, prev can
  assert(line != NULL);
  block_t *b = malloc(sizeof(block_t));
  assert(b != NULL);

  if (prev != NULL) { // there is a former block
    // copy all data from the previous block
    memcpy(b, prev, sizeof(block_t));
    // set prev/next references
    b->prev = prev;
    prev->next = b;
  } else { // this is the first block
    b->prev = NULL;
  }

  // fields that must be calculated, not inherited from prev
  b->length = 0.0;
  b->target = point_new();
  b->delta = point_new();
  // allocate memory for referenced valus
  b->prof = malloc(sizeof(block_profile_t));
  assert(b->prof != NULL);
  // copy line into b->line
  asprintf(&(b->line), "%s", line);
  return b;
}

void block_free(block_t *block) {
  assert(block != NULL);
  // free allocated fields, if they are not null
  if (block->line != NULL) free(block->line);
  if (block->prof != NULL) free(block->prof);
  // free the block itself
  free(block);
}

index_t block_parse(block_t *block) {
  assert(block != NULL);
  index_t i = 0; //number of words counter
  char *line, *word, *tofree;

  // strdup allocates a new string and does the copy
  // so we need to keep track of the original memory
  // pointer and free it later
  tofree = line = strdup(block->line);
  assert(line != NULL);

  while ((word = strsep(&line, " ")) != NULL) {
    block_set_field(block, word[0], word + 1);
    i++;
  }
  free(tofree);
  return i;
}

data_t block_lambda(block_t *block, data_t time) {

}

point_t block_interpolate(block_t *block, data_t lambda) {

}

void block_print(block_t *block, FILE *out) {

}
