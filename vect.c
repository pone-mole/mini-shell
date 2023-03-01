
/**
 * Vector implementation.
 *
 * - Implement each of the functions to create a working growable array (vector).
 * - Do not change any of the structs
 * - When submitting, You should not have any 'printf' statements in your vector 
 *   functions.
 *
 * IMPORTANT: The initial capacity and the vector's growth factor should be 
 * expressed in terms of the configuration constants in vect.h
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vect.h"

/** Main data structure for the vector. */
struct vect {
  char **data;             /* Array containing the actual data. */
  unsigned int size;       /* Number of items currently in the vector. */
  unsigned int capacity;   /* Maximum number of items the vector can hold before growing. */
};

/** Construct a new empty vector. */
/** Return the new empty vector. */
vect_t *vect_new() {

  vect_t *v = malloc(sizeof(vect_t));
  v->capacity = VECT_INITIAL_CAPACITY;
  v->data = calloc(v->capacity, sizeof(char*));
  v->size = 0;

  return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {
  while (v->size > 0) {
    vect_remove_last(v);
  }
  free(v->data);
  free(v);
}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  return v->data[idx];
}

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. */
char *vect_get_copy(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  // copy the value at the designated index and return it
  char* result = malloc(sizeof(char*) * strlen(v->data[idx]) + 1);
  strcpy(result, v->data[idx]);

  return result;
}

/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt) {
  assert(v != NULL);
  assert(idx < v->size);
  assert(elt != NULL);

  free(v->data[idx]);                      // free the pre-existing data
  v->data[idx] = malloc(strlen(elt) + 1);  // allocate new mem space
  strcpy(v->data[idx], elt);               // copy new element into idx space
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
  assert(v != NULL);
  assert(elt != NULL);

  //expand the data spread if the size is equal to the capacity
  if ((v->size == v->capacity) && (v->capacity != VECT_MAX_CAPACITY)) {
    v->data = realloc(v->data, VECT_GROWTH_FACTOR * v->capacity * sizeof(char*));
    // updates the capacity
    v->capacity = v->capacity * VECT_GROWTH_FACTOR;
  }

  // add item to the data spread
  v->data[v->size] = malloc(strlen(elt) + 1);        // allocate mem for the element plus 1 space for strcpy
  strcpy(v->data[v->size], elt);
  v->size++;
}

/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v) {
  assert(v != NULL);
  free(v->data[v->size -1]);
  v->size--;
}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v) {
  assert(v != NULL);
  return v->size;
}

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v) {
  assert(v != NULL);
  return v->capacity;
}

