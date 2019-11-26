#include "block.h"

void block_set_field(block_t *b, char cmd, char *arg) {

}


block_t *block_new(char *line, block_t *prev) {
  block_t *b = malloc(sizeof(block_t));
  assert(b != NULL);

  if (prev != NULL) {
    memcpy(b, prev, sizeof(block_t));
    b->prev = prev;
    prev->next = b;
  } else { // this is the firt block
    b->prev = NULL;
  }
  b->target = point_new();
  b->prof = malloc(sizeof(block_profile_t));
  b->delta = point_new();
  b->length = 0.0;
  asprintf(b->line, "%s", line);
  return b;
}

void block_free(block_t *block) {
  assert(block != NULL);
  if (block->line != NULL) free(block->line);
  if (block->prof != NULL) free(block->prof);
  free(block);
}

index_t block_parse(block_t *block) {
  assert(block != NULL);
  index_t i = 0;
  char *line, *word, *tofree;

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