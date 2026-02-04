#define INITIAL_EVENT_CAP 1024

#include <stdlib.h>

#include "config.h"

void init_event_list(EventList* list) {
    list->size = 0;
    list->capacity = INITIAL_EVENT_CAP;
    list->data = malloc(sizeof(RelaxEvent) * list->capacity);
}

void free_event_list(EventList* list) {
    free(list->data);
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

void record_event(EventList* list, Edge* edge, int successful) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->data = realloc(list->data,
                             sizeof(RelaxEvent) * list->capacity);
    }

    list->data[list->size].edge = edge;
    list->data[list->size].successful = successful;
    list->size++;
}

