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

// the heuristic is a simple Euclidian distance
float calculate_heuristic(Node* node, Node* end) {
    float dx = node->pos.x - end->pos.x;
    float dy = node->pos.y - end->pos.y;
    return sqrtf(dx * dx + dy * dy) * 1000;
    /*
     * the multiplying by 1000 here is needed because the h_cost would otherwise
     * be too small to outweight the g_costs. I don't really like the use of
     * this constant here as I do find it quite arbirary and so I hope that the
     * choice of 1000 works consistently on different maps but if it does happen
     * that A* is not performing properly then consider changing its value.
     *
     * (to know what a reaosnable value is just print out the g and h costs of
     * several nodes to ensure that there is not one value that is disproportionatly
     * outbalancing the other).
     */
}

void a_star(Node* nodes, int numNodes, Node* start, Node* target, EventList* events) {
    reset_nodes(nodes, numNodes);

    MinHeap heap;
    heap_init(&heap, 1024);

    start->g_cost = 0.0f;
    start->h_cost = calculate_heuristic(start, target);
    start->f_cost = start->g_cost + start->h_cost;

    heap_push(&heap, start, start->f_cost);

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


            if (neighbor->visited) continue;

            record_event(events, e, 0); // examined edge

            float tentative_g = current->g_cost + e->weight;

            if (tentative_g < neighbor->g_cost) {

                neighbor->g_cost = tentative_g;
                neighbor->h_cost = calculate_heuristic(neighbor, target);
                neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;

                neighbor->came_from = e;

                heap_push(&heap, neighbor, neighbor->f_cost);
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

void paint_path(SDL_Renderer* renderer, Camera camera, Node* target, 
        SDL_Texture* tex) {
    SDL_SetRenderDrawColor(renderer, YELLOW.r, YELLOW.g, YELLOW.b, YELLOW.a);

    SDL_SetTextureAlphaMod(tex, 200); // 0 is invisible, 255 is solid
                                      
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_ADD);

    Node* current = target;
    while (current->came_from != NULL) {
        Edge* e = current->came_from;

        draw_road_thick(renderer, camera, *e->road, tex);
        //draw_road(renderer, camera, *e->road);

        current = e->start;
    }

    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
}
