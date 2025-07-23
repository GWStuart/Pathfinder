#ifndef LOAD_DATA_H
#define LOAD_DATA_H

#include <stdbool.h>
#include "config.h"

// helper function that prints the given road in a nice format
void print_pos(Pos pos, bool newline);

// helper function that prints the given node in a nice format
void print_node(Node node, bool newline);

// longer version of print_node function. Also shows neighbour positions
void print_node_long(Node node);

// helper function that prints the given road in a nice format
void print_road(Road road, bool newline);

/* load_nodes()
 * ------------
 * load the nodes array using data from the specified filename
 * filename: the filename to be loaded
 * nodes: pointer to where the nodes array should be stored
 */
int load_nodes(char* filename, Node** nodes);

/* load_nodes()
 * ------------
 * load the nodes array using data from the specified filename
 * filename: the filename to be loaded
 * nodes: pointer to where the nodes array should be stored
 */
int load_roads(char* filename, Node* nodes, Road** roads);

#endif
