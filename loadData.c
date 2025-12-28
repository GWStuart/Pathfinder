#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "loadData.h"
#include "stringUtils.h"

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


// see header
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


// see header
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
