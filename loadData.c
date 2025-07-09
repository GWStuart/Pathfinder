#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

// holds a coorrdinate position
typedef struct {
    double x;
    double y;
} Pos;

/* 
 * struct that holds each node in the road network
 * pos: the coordinate position of the node
 * numNeighbours: the total number of neighbouring nodes
 * neighbours: an array containing all nodes that neighbour the given node
 */
typedef struct Node {
    Pos pos;
    int numNeighbours;
    struct Node** neighbours;
} Node;

/*
 * struct that holds road data. All roads are uniquely defined by their start and
 * end nodes.
 * start: the node of the starting point of the road
 * end: the node of the end point of the road
 * path: array of coordinate positions that define the road path
 */
typedef struct {
    Node start;
    Node end;
    Pos* path;
} Road;

// helper function that prints the given node in a nice format
void print_node(Node node, bool newline) {
    printf("Node(%lf, %lf, neighbours=%d)", node.pos.x, node.pos.y, 
            node.numNeighbours);

    // print a newline if required
    if (newline) {
        printf("\n");
    }
}

// longer version of print_node function. Also shows neighbour positions
void print_node_long(Node node) {
    printf("Node(%lf, %lf, [", node.pos.x, node.pos.y);

    // print information for each neighbour
    for (int i = 0; i < node.numNeighbours; i++) {
        Node* n = node.neighbours[i];
        printf(" (%lf,%lf)", n->pos.x, n->pos.y);
    }
    printf(" ]\n");
}

// counts the number of occurences of the specified character in the string
int count_occurences(char* string, char character) {
    int count = 0;
    for (int i=0; i<strlen(string); i++) {
        if (string[i] == character) {
            count++;
        }
    }
    return count;
}

/* returns a new string pointer that starts 1 chracter afrer the specified
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
 */
void populate_neighbours(char* string, Node* node, Node** nodes) {
    int numNeighbours = count_occurences(string, ',') + 1;

    // create the neighbours array
    node->neighbours = malloc(sizeof(Node*) * numNeighbours);

    string = split_string(string, '[');

    char* newString;
    int num;
    for (int i = 0; i < numNeighbours-1; i++) {
        newString = split_string(string, ',');
        num = atoi(string);
        node->neighbours[i] = &((*nodes)[num]);
        string = newString;
    }

    // add the last node
    num = atoi(string);
    node->neighbours[numNeighbours - 1] = &((*nodes)[num]);

    // record the number of neighbours
    node->numNeighbours = numNeighbours;
}

/* load_nodes()
 * ------------
 * load the nodes array using data from the specified filename
 * filename: the filename to be loaded
 * nodes: pointer to where the nodes array should be stored
 */
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
        populate_neighbours(string, &(*nodes)[i], nodes);
        i++;
    }

    return numNodes;
}

int main() {
    // populate the nodes array
    Node* nodes;
    int numNodes = load_nodes("data/no.nodes", &nodes);

    // print the nodes out to be checked visually
    for (int i=0; i<numNodes; i++) {
        //print_node(nodes[i], true);
        print_node_long(nodes[i]);
    }
}
