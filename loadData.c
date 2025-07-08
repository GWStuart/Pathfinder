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
struct that holds each node in the road network
pos: the coordinate position of the node
numNeighbours: the total number of neighbouring nodes
neighbours: an array containing all nodes that neighbour the given node
*/
typedef struct Node {
    Pos pos;
    int numNeighbours;
    struct Node** neighbours;
} Node;

/*
struct that holds road data. All roads are uniquely defined by their start and
end nodes.
start: the node of the starting point of the road
end: the node of the end point of the road
path: array of coordinate positions that define the road path
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

// assumes that the character is in the string and is not the last character
char* move_to_char(char* string, char character) {
    while (string[0] != character) {
        string++;
    }
    string++;
    return string;
}

Pos extract_pos(char* string) {
    char* abc = move_to_char(string, '(');
    printf("%s", abc);
    return (Pos){1, 3};
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
        (*nodes)[i].pos = extract_pos(buffer);
        (*nodes)[i].numNeighbours = count_occurences(buffer, '(');

        i++;
    }


//    for (int i=0; i<10; i++) {
//        nodes[i].pos = (Pos){10, 1};
//    }

    return numNodes;
}

int main() {
    // populate the nodes array
    Node* nodes;
    int numNodes = load_nodes("data/temp2.nodes", &nodes);

    for (int i=0; i<numNodes; i++) {
        print_node(nodes[i], true);
    }
}
