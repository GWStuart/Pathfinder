/*
 * loadData.c
 *
 * Program responsbile for loading GeoJSON map data. Implements the
 * functions `load_roads()` and `load_nodes()` that are used in
 * cvisualiser.c.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "loadData.h"

#define MAX_BUFFER_SIZE 8192


// helper function to print a position struct
void print_pos(Pos pos, bool newline) {
    printf("(%lf, %lf)", pos.x, pos.y);

    // print a newline if required
    if (newline) {
        printf("\n");
    }
}


// helper function to print a node struct
void print_node(Node node, bool newline) {
    printf("Node(%lf, %lf, neighbours=%d)", node.pos.x, node.pos.y, 
            node.numNeighbours);

    // print a newline if required
    if (newline) {
        printf("\n");
    }
}


// helper function to print a node struct in long form
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


// helper function to print a road struct
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


// counts the number of occurences of the specified character in the string
int count_occurences(char* string, char character) {
    int count = 0;
    for (size_t i=0; i<strlen(string); i++) {
        if (string[i] == character) {
            count++;
        }
    }
    return count;
}


/* returns a new string pointer that starts 1 chracter after the specified
 * character. Note that this function assumes that the specified character is
 * in the string.
 */
char* move_to_char(char* string, char character) {
    while (string[0] != character) {
        string++;
    }
    string++;
    return string;
}

/* returns the index of the string containing the specified character. This
 * function assumes the character is present somewhere in the string
 */
int index_of(char* string, char character) {
    int i = 0;
    while (string[i] != character) {
        i++;
    }
    return i;
}

/* partitions the given string so that it is split at the specified character.
 * this functiton returns a pointer to the second half of the partition
 */
char* split_string(char* string, char character) {
    while (string[0] != character) {
        string++;
    }
    string[0] = '\0';
    string++;
    return string;
}

/* extracts the first position struct form the given string. Returns a pointer
 * remaining portion of the string
 */
char* extract_pos(char* string, Pos* pos) {
    char* num1String = move_to_char(string, '(');
    char* num2String = split_string(num1String, ',');

    int endOfString = index_of(num2String, ')');
    num2String[endOfString] = '\0';

    // convert the strings to doubles
    double num1 = atof(num1String);
    double num2 = atof(num2String);

    // save the position struct and return the moved string
    *pos = (Pos){num1, num2};
    return num2String + endOfString + 1;
}

/* populate_neighbours()
 * ---------------------
 * populates a nodes neighbours array with pointer to all nodes that it is
 * connected to
 * string: the string from which the information is to be extracted
 * node: the node whose neighbours are to be populated
 * nodes: the full array of all nodes in the graph
 *
 * returns a char* pointing towards the remaining portion of  the string that
 * was not used.
 */
char* populate_neighbours(char* string, Node* node, Node** nodes) {
    string = split_string(string, '[');
    char* remaining = split_string(string, '['); // cutoff remaining characters

    // create the neighbours array
    int numNeighbours = count_occurences(string, ',') + 1;
    node->neighbours = malloc(sizeof(Neighbour) * numNeighbours);

    char* newString;
    int num;
    for (int i = 0; i < numNeighbours-1; i++) {
        newString = split_string(string, ',');
        num = atoi(string);
        node->neighbours[i].node = &((*nodes)[num]);
        node->neighbours[i].weight = 1.24f; // TODO: do this later
        string = newString;
    }

    // add the last node
    num = atoi(string);
    node->neighbours[numNeighbours - 1].node = &((*nodes)[num]);
    node->neighbours[numNeighbours - 1].weight = 1.24f; // TODO: do this later

    // record the number of neighbours
    node->numNeighbours = numNeighbours;

    return remaining;
}

int load_nodes(char* filename, Node** nodes) {
    FILE* file = fopen(filename, "r");

    char buffer[MAX_BUFFER_SIZE];

    // extract the number of nodes specified by the file
    fgets(buffer, sizeof(buffer), file);
    int numNodes = atoi(buffer);

    // generate the noes array
    *nodes = (Node*)malloc(sizeof(Node) * numNodes);

    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        char* string = extract_pos(buffer, &(*nodes)[i].pos);
        string = populate_neighbours(string, &(*nodes)[i], nodes);
        //printf("%s", string); // TODO: probably don't need to return the string
        // in future. The populate_neighbours function should be able to handle
        // all of it.
        i++;
    }

    // cleanup
    fclose(file);

    return numNodes;
}


/* load_path()
 * ---------------------
 * loads in the path nodes from the given string and adds them to the specified
 * road.
 * string: the string from which the information is to be extracted
 * road: the road associated with the path data
 */
void load_path(char* string, Road* road) {
    int numCoords = count_occurences(string, '(');

    // extract all points from the string
    Pos* coords = malloc(sizeof(Pos) * numCoords);
    for (int i=0; i<numCoords; i++) {
        Pos pos;
        string = extract_pos(string, &pos);
        coords[i] = pos;
    }

    // set the road data
    road->pathCount = numCoords;
    road->path = coords;
}


int load_roads(char* filename, Node* nodes, Road** roads) {
    FILE* file = fopen(filename, "r");

    char buffer[8192];

    // extract the number of roads specified by the file
    fgets(buffer, sizeof(buffer), file);
    int numRoads = atoi(buffer);

    // generate the road array
    *roads = (Road*)malloc(sizeof(Road) * numRoads);

    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        char* string2 = split_string(buffer, ' ');
        char* path = split_string(string2, ' ');

        int startIndex = atoi(buffer);
        int endIndex = atoi(string2);

        (*roads)[i].start = nodes[startIndex];
        (*roads)[i].end = nodes[endIndex];

        // load the path
        load_path(path, &(*roads)[i]);

        i++;
    }

    // cleanup
    fclose(file);

    return numRoads;
}
