#ifndef PATHFINDER_CONFIG_H
#define PATHFINDER_CONFIG_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// define SDL screen sizes
#define WIDTH 800
#define HEIGHT 600

// define camera constants
#define PAN_SPEED 5
#define ZOOM_FACTOR 1.1
#define ZOOM_DIFF ZOOM_FACTOR - 1
#define INIT_SCALE WIDTH

// other constants
#define PADDING 5
#define FONT_SIZE 18

// make predefined SDL colours availble to all including files
extern const SDL_Color BG;
extern const SDL_Color WHITE;
extern const SDL_Color RED;
extern const SDL_Color BLUE;

// holds a coorrdinate position
typedef struct {
    double x;
    double y;
} Pos;

// forward decleration of Node
typedef struct Node Node;

/* struct used to hold node neighbours
 * node: a pointer to the neighbouring node
 * weight: the weight of that connection
 */
typedef struct {
    Node* node;
    double weight;
} Neighbour;

/* struct that holds each node in the road network
 * pos: the coordinate position of the node
 * numNeighbours: the total number of neighbouring nodes
 * neighbours: an array containing node neighbour structs
 */
typedef struct Node {
    Pos pos;
    int numNeighbours;
    Neighbour* neighbours;
} Node;

/* struct that holds road data. All roads are uniquely defined by their start 
 * and end nodes.
 * start: the node of the starting point of the road
 * end: the node of the end point of the road
 * pathCount: the number of elements in the path array
 * path: array of coordinate positions that define the road path
 */
typedef struct {
    Node start;
    Node end;
    int pathCount;
    Pos* path;
} Road;

/* struct used to hold camera information */
typedef struct {
    double x;
    double y;
    double zoom;
} Camera;

/* struct used to hold useful data structures required for rendering
 * renderer: the SDL3 renderer struct
 * camera: struct containing camera info
 * showNodes: if set then intersection nodes are rendered
 * showHighlights: if set then road highlights are rendered
 */
typedef struct {
    SDL_Renderer* renderer;
    Camera* camera;
    bool showNodes;
    bool showHighlihts;
} Display;

// in future I think this struct will hold more than just the start and end
// haven't fully figured it out yet
typedef struct {
    Node start;
    Node end;
} Pathfinder;

#endif
