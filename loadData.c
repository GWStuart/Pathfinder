#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "loadData.h"
#include "stringUtils.h"

// can remove later
#include "printUtils.h"

#define MAX_BUFFER_SIZE 8192


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

// see header
int load_nodes(char* filename, Node** nodes) {
    FILE* file = fopen(filename, "r");

    char buffer[MAX_BUFFER_SIZE];

    // extract the number of nodes specified by the file
    fgets(buffer, sizeof(buffer), file);
    int numNodes = atoi(buffer);

    // generate the nodes array
    *nodes = (Node*)malloc(sizeof(Node) * numNodes);

    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        // extract and save the node position
        char* string = extract_pos(buffer, &(*nodes)[i].pos);

        // extract and save the number of connecting edges
        int num_edges = atoi(string);
        (*nodes)[i].num_edges = num_edges;

        // initialise the next edge to 0
        (*nodes)[i].next_edge = 0;

        // setup the edges array
        (*nodes)[i].edges = (Edge**)malloc(sizeof(Edge*) * num_edges);

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


// see header
int load_roads(char* filename, Road** roads) {
    FILE* file = fopen(filename, "r");

    char buffer[MAX_BUFFER_SIZE];

    // extract the number of roads specified by the file
    fgets(buffer, sizeof(buffer), file);
    int numRoads = atoi(buffer);

    // generate the road array
    *roads = (Road*)malloc(sizeof(Road) * numRoads);

    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        load_path(buffer, &(*roads)[i]);

        i++;
    }

    // cleanup
    fclose(file);

    return numRoads;
}

// see header
int load_edges(char* filename, Node* nodes, Road* roads, Edge** edges) {
    FILE* file = fopen(filename, "r");

    char buffer[MAX_BUFFER_SIZE];

    // extract the number of edges specified by the file
    fgets(buffer, sizeof(buffer), file);
    int numEdges = atoi(buffer);

    // generate the road array
    *edges = (Edge*)malloc(sizeof(Edge) * numEdges);

    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        // extract the parts
        char* part1 = split_string(buffer, ' ');
        char* part2 = split_string(part1, ' ');

        // convert data types
        int from = atoi(buffer);
        int to = atoi(part1);
        float weight = strtof(part2, NULL);

        // populate the edge
        (*edges)[i].start = nodes + from;
        (*edges)[i].end = nodes + to;
        (*edges)[i].weight = weight;
        (*edges)[i].road = roads + i;

        i++;
    }

    // cleanup
    fclose(file);

    return numEdges;
}

