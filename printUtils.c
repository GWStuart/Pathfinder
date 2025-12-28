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
    printf("Node(%lf, %lf, neighbours=%d)", node.pos.x, node.pos.y, 
            node.numNeighbours);

    // print a newline if required
    if (newline) {
        printf("\n");
    }
}


void print_node_long(Node node) {
    printf("Node(%lf, %lf, [", node.pos.x, node.pos.y);

    // print information for each neighbour
    for (int i = 0; i < node.numNeighbours; i++) {
        Node* n = node.neighbours[i].node;
        printf(" (%lf,%lf,w=%lf)", n->pos.x, n->pos.y, 
                node.neighbours[i].weight);
    }
    printf(" ]\n");
}


void print_road(Road road, bool newline) {
    printf("Road(from <%lf,%lf> to <%lf,%lf>)\n", road.start.pos.x,
            road.start.pos.x, road.end.pos.x, road.end.pos.y);

    printf("Num paths: %d\nPath: ", road.pathCount);
    for (int i=0; i<road.pathCount; i++) {
        printf("<%lf,%lf> ", road.path[i].x, road.path[i].y);
    }

    // print a newline if required
    if (newline) {
        printf("\n");
    }
}

