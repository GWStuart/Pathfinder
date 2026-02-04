#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "math.h"
#include "printUtils.h"
#include "renderUtils.h"
#include "heap.h"
#include "animation.h"

// resets all node values 
void reset_nodes(Node* nodes, int numNodes) {
    for (int i = 0; i < numNodes; i++) {
        nodes[i].g_cost = INFINITY;
        nodes[i].visited = 0;
        nodes[i].came_from = NULL;
    }
}

void dijkstra(Node* nodes, int numNodes, Node* start, Node* target, EventList* events) {
    reset_nodes(nodes, numNodes);

    MinHeap heap;
    heap_init(&heap, 1024);

    start->g_cost = 0.0f;
    heap_push(&heap, start, 0.0f);

    while (!heap_empty(&heap)) {

        HeapNode h = heap_pop(&heap);
        Node* current = h.node;

        if (current->visited)
            continue;

        current->visited = 1;

        if (current == target)
            break;

        for (int i = 0; i < current->num_edges; i++) {
            Edge* e = current->edges[i];
            Node* neighbor = e->end;


            if (neighbor->visited)
                continue;

            record_event(events, e, 0); // examined edge

            float alt = current->g_cost + e->weight;

            if (alt < neighbor->g_cost) {
                neighbor->g_cost = alt;
                neighbor->came_from = e;
                heap_push(&heap, neighbor, alt);

                record_event(events, e, 1); // successful edge
            }
        }
    }

    heap_free(&heap);
}

void print_path(Node* target) {
    Node* current = target;

    while (current->came_from != NULL) {
        Edge* e = current->came_from;

        printf("Edge from: ");
        print_pos(e->start->pos, false);
        printf(" to ");
        print_pos(e->end->pos, true);

        current = e->start;
    }
}

void paint_path(SDL_Renderer* renderer, Camera camera, Node* target) {
    SDL_SetRenderDrawColor(renderer, YELLOW.r, YELLOW.g, YELLOW.b, YELLOW.a);

    Node* current = target;
    while (current->came_from != NULL) {
        Edge* e = current->came_from;

        draw_road(renderer, camera, *e->road);

        current = e->start;
    }
}
