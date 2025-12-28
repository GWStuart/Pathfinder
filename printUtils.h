/* 
 * printUtils.c
 *
 * contains useful print utilities to visualise the various data
 * structs.
 *
 */

#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include "config.h"

// helper function to print a position struct
void print_pos(Pos pos, bool newline);

// helper function to print a node struct
void print_node(Node node, bool newline);

// helper function to print a node struct in long form
void print_node_long(Node node);

// helper function to print a road struct
void print_road(Road road, bool newline);

#endif
