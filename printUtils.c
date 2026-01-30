/* 
 * printUtils.c
 *
 * contains useful print utilities to visualise the various data
 * structs.
 *
 */

#include <stdio.h>
#include "config.h"


void print_pos(Pos pos, bool newline) {
    printf("(%lf, %lf)", pos.x, pos.y);

    // print a newline if required
    if (newline) {
        printf("\n");
    }
}


void print_node(Node node, bool newline) {
    printf("Node(%lf, %lf, numEdges=%d)", node.pos.x, node.pos.y, 
            node.num_edges);

    // print a newline if required
    if (newline) {
        printf("\n");
    }
}

void print_road(Road road, bool newline) {
    printf("Road (%d): ", road.pathCount);

    for (int i=0; i<road.pathCount; i++) {
        printf("<%lf,%lf> ", road.path[i].x, road.path[i].y);
    }

    // print a newline if required
    if (newline) {
        printf("\n");
    }
}

