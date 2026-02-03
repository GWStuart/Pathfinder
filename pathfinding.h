#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "config.h"

/* dijkstra()
 * ----------
 * Basic implementation of the dijkstra search algorithm
 */
//void dijkstra(Node* nodes, int numNodes, Node* start);
void dijkstra(Node* nodes, int numNodes, Node* start, Node* target);

/* print_path()
 * ------------
 * Traces back the path from the target node
 */
void print_path(Node* target);


void paint_path(SDL_Renderer* renderer, Camera camera, Node* target);

#endif
