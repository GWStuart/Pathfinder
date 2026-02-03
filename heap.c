#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#define PARENT(i) (((i) - 1) / 2)
#define LEFT(i)   (2*(i) + 1)
#define RIGHT(i)  (2*(i) + 2)


void heap_init(MinHeap* heap, int initial_capacity) {
    heap->size = 0;
    heap->capacity = initial_capacity;
    heap->data = malloc(sizeof(HeapNode) * initial_capacity);
}

void heap_resize(MinHeap* heap) {
    heap->capacity *= 2;
    heap->data = realloc(heap->data, sizeof(HeapNode) * heap->capacity);
}

void heap_push(MinHeap* heap, Node* node, float priority)
{
    // resize if the heap is full
    if (heap->size >= heap->capacity) heap_resize(heap);

    int i = heap->size++;
    heap->data[i].node = node;
    heap->data[i].priority = priority;

    // Bubble up
    while (i > 0) {
        int p = PARENT(i);

        if (heap->data[p].priority <= heap->data[i].priority)
            break;

        // Swap
        HeapNode temp = heap->data[p];
        heap->data[p] = heap->data[i];
        heap->data[i] = temp;

        i = p;
    }
}

HeapNode heap_pop(MinHeap* heap) {
    HeapNode min = heap->data[0];

    heap->data[0] = heap->data[--heap->size];

    int i = 0;

    // Bubble down
    while (1) {
        int l = LEFT(i);
        int r = RIGHT(i);
        int smallest = i;

        if (l < heap->size &&
            heap->data[l].priority < heap->data[smallest].priority)
            smallest = l;

        if (r < heap->size &&
            heap->data[r].priority < heap->data[smallest].priority)
            smallest = r;

        if (smallest == i)
            break;

        HeapNode temp = heap->data[i];
        heap->data[i] = heap->data[smallest];
        heap->data[smallest] = temp;

        i = smallest;
    }

    return min;
}

int heap_empty(MinHeap* heap) {
    return heap->size == 0;
}

void heap_free(MinHeap* heap) {
    free(heap->data);
}

