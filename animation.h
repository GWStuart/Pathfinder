#ifndef EVENT_LIST_H
#define EVENT_LIST_H

#include "config.h"

void init_event_list(EventList* list);

void free_event_list(EventList* list);

void record_event(EventList* list, Edge* edge, int successful);

#endif
