/*
 * sstack.c
 *
 *  Created on: May 22, 2020
 *      Author: justin
 */

// This is the sstack ADT module implementation

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "sstack.h"


// Structure definition

struct sstack {
  int len;
  int maxlen;
  char **data;
};


// my_strdup(s) makes a duplicate of s
// effects: allocates memory (caller must free)
// requires: s is a valid pointer
//           s points to a valid string [not asserted]
// time: O(n), where n is the length of s

static char *my_strdup(const char *s) {
  assert(s);
  char *newstr = malloc((strlen(s) + 1) * sizeof(char));
  strcpy(newstr, s);

  return newstr;
}


// see sstack.h for sstack function documentations


struct sstack *sstack_create(void) {
  struct sstack *to_return = malloc(sizeof(struct sstack));

  to_return->len = 0;
  to_return->maxlen = 1;
  to_return->data = malloc(to_return->maxlen * sizeof(char*));

  return to_return;
}


bool sstack_is_empty(const struct sstack *ss) {
  assert(ss);
  return ss->len == 0;
}

int sstack_get_len(const struct sstack *ss) {
  assert(ss);
  return ss->len;
}
const char *sstack_top(const struct sstack *ss) {
  assert(ss);
  assert(!sstack_is_empty(ss));
  const char *top = ss->data[ss->len - 1];

  return top;
}


char *sstack_pop(struct sstack *ss) {
  assert(ss);
  assert(!sstack_is_empty(ss));

  char *top = ss->data[ss->len - 1];
  ss->len--;

  if (ss->len * 4 == ss->maxlen) {
    ss->maxlen /= 2;
    ss->data = realloc(ss->data, ss->maxlen * sizeof(char*));
  }
  return top;
}


void sstack_push(const char *str, struct sstack *ss) {
  assert(str);
  assert(ss);

  if (ss->len >= ss->maxlen) {
    ss->maxlen *= 2;
    ss->data = realloc(ss->data, ss->maxlen * sizeof(char*));
  }

  ss->data[ss->len] = my_strdup(str);
  ss->len++;
}


void sstack_destroy(struct sstack *ss) {
  assert(ss);

  for (int i = 0; i < ss->len; i++) {
    free(ss->data[i]);
  }
  free(ss->data);
  free(ss);
}


void sstack_status(const struct sstack *ss) {
  assert(ss);

  printf("status: %d/%d\n", ss->len, ss->maxlen);
}


