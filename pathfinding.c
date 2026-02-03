#include "config.h"
#include "math.h"
#include "printUtils.h"
#include "renderUtils.h"

#include <stdio.h>

// resets all node values 
void reset_nodes(Node* nodes, int numNodes) {
    for (int i = 0; i < numNodes; i++) {
        nodes[i].g_cost = INFINITY;
        nodes[i].visited = 0;
        nodes[i].came_from = NULL;
    }
}

void dijkstra(Node* nodes, int numNodes, Node* start) {
    reset_nodes(nodes, numNodes);

    start->g_cost = 0.0f;

    while (1) {

        // 1. Find unvisited node with smallest distance
        Node* current = NULL;
        float best = INFINITY;

        for (int i = 0; i < numNodes; i++) {
            if (!nodes[i].visited && nodes[i].g_cost < best) {
                best = nodes[i].g_cost;
                current = &nodes[i];
            }
        }

        // No reachable nodes left
        if (!current)
            break;

        current->visited = 1;

        // 2. Relax outgoing edges
        for (int i = 0; i < current->num_edges; i++) {

            Edge* e = current->edges[i];
            Node* neighbor = e->end;

            if (neighbor->visited)
                continue;

            float alt = current->g_cost + e->weight;

            if (alt < neighbor->g_cost) {
                neighbor->g_cost = alt;
                neighbor->came_from = e;
            }
        }
    }
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
