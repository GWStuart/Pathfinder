/*
 * loadData.c
 *
 * Program responsbile for loading GeoJSON map data. Implements the
 * functions `load_roads()` and `load_nodes()` that are used in
 * cvisualiser.c.
 * 
 */

#ifndef LOAD_DATA_H
#define LOAD_DATA_H

#include <stdbool.h>
#include "config.h"

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
