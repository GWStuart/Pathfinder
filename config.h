/*
 * config.h
 *
 * File used to hold most structs and macros used in the program 
 * so that they can all be found a loaded from a centralised place.
 *
 */

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
extern const SDL_Color YELLOW;
extern const SDL_Color PURPLE;
extern const int GRAD_START[3];
extern const int GRAD_END[3];

// holds a coordinate position
typedef struct {
    double x;
    double y;
} Pos;


typedef struct Edge Edge;


typedef struct {
    Pos pos;

    int num_edges;
    int next_edge; // next edge to be populated
                   //
    Edge** edges;

    // additional data for the pathfinding state
    float g_cost; // actual distance travelled
    float h_cost;
    float f_cost;
    int visited;
    Edge* came_from;
} Node;


typedef struct {
    Pos* path;
    int pathCount;
} Road;


typedef struct Edge {
    Node* start;
    Node* end;
    float weight;

    Road* road;
} Edge;


// haven't used this yet but it might be useful in future
typedef struct {
    Node* nodes;
    int num_nodes;

    Edge* edges;
    int num_edges;

    Road* roads;
    int num_roads;
} Graph;


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


typedef struct {
    Node* node;
    float priority;
} HeapNode;

typedef struct {
    HeapNode* data;
    int size;
    int capacity;
} MinHeap;

typedef struct {
    Edge* edge;
    int successful;   // 0 = examined, 1 = relaxed
                      // (I don't actually use the "successful attribute
                      // right now)
} RelaxEvent;

typedef struct {
    RelaxEvent* data;
    int size;
    int capacity;
} EventList;

#endif
