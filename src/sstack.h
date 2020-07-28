/*
 * sstack.h
 *
 *  Created on: May 22, 2020
 *      Author: justin
 */

#ifndef SRC_SSTACK_H_
#define SRC_SSTACK_H_
// This is a module that provides a stack of strings [sstack] ADT
// where each string is dynamically allocated (copied) when pushed

// all sstack parameters must be valid (non-null)

// times: n is the length of the stack (number of items)
//        m is the length of the string (parameter)

#include <stdbool.h>

struct sstack;

// sstack_create() creates a new empty string stack [sstack]
// note: calling sstack_status after sstack_create displays "status: 0/1\n"
//       (see sstack_status below)
// effects: allocates memory (client must call sstack_destroy)
// time: O(1)
struct sstack *sstack_create(void);

// sstack_is_empty(ss) determines if ss is empty
// time: O(1)
bool sstack_is_empty(const struct sstack *ss);

// sstack_get_len(ss) returns the len of the sstack
// time: O(1)
int sstack_get_len(const struct sstack *ss);

// sstack_top(ss) returns a (const) pointer the top item in ss
// requires: ss is not empty
// time: O(1)
const char *sstack_top(const struct sstack *ss);

// sstack_pop(ss) returns and pops (removes) the top item in stack ss
// note: caller must free the returned string
// requires: ss is not empty
// effects: modifies ss
// time: O(1) [amortized]
char *sstack_pop(struct sstack *ss);

// sstack_push(str, ss) pushes str on top of ss
// note: makes a 'copy' of str (caller must free when popped)
// requires: str is a valid (non-null) string
// effects: modifies ss
//          allocates memory (caller must free when popped)
// time: O(m) [amortized]
void sstack_push(const char *str, struct sstack *ss);

// stack_destroy(ss) frees all memory for ss (including any strings)
// effects: ss is no longer valid
// time: O(n)
void sstack_destroy(struct sstack *ss);

// stack_status(ss) displays information about ss
// note: uses printf("status: %d/%d\n", length, maximum_capacity)
//       to display the current (length/maximum_capacity) of the stack
// effects: produces output
// time: O(1)
void sstack_status(const struct sstack *ss);



#endif /* SRC_SSTACK_H_ */
