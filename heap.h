#ifndef HEAP_H
#define HEAP_H

#include "config.h"

/* heap_init()
 * ----------
 * initialises the heap with the given capacity
 */
void heap_init(MinHeap* heap, int initial_capacity);

/* heap_resize()
 * -------------
 * Resizes the heap doubling the storage capacity
 */
void heap_resize(MinHeap* heap);

void heap_push(MinHeap* heap, Node* node, float priority);

HeapNode heap_pop(MinHeap* heap);

int heap_empty(MinHeap* heap);

void heap_free(MinHeap* heap);

#endif
