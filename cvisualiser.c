/*
 * cvisualiser.c
 *
 * Main program entry point.
 *
 * This file is responsible for:
 *    - holding the main() function
 *    - initialise and setup SDL window
 *    - unite functionality across other files
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "loadData.h"
#include "config.h"
#include "renderUtils.h"
#include "printUtils.h"
#include "pathfinding.h"
#include "animation.h"

// can remove this include later
#include <sys/resource.h>
#include <stdlib.h>

// there are a few states in which the software can be.
// STATE_SELECT_START --> select pathfinder start point
// STATE_SELECT_END   --> select pathfinder end point
// STAE_HOLD          --> a waiting state before running the pathfinder
// STATE_PATHFIND     --> running the pathfinder
enum {
    STATE_SELECT_START = 1,
    STATE_SELECT_END = 2,
    STATE_HOLD = 3,
    STATE_PATHFIND = 4,
    STATE_ANIMATE = 5,
};

// specify the algorithm that is to be used
enum {
    ALG_DIJKSTRA = 1,
    ALG_A_STAR = 2,
};

// define string constants
const char* const MSG_START = "Select a starting point";
const char* const MSG_END = "Select an end point";
const char* const MSG_HOLD = "Press <space> to start";

// temporary function used to check what the memory usage is.
// this function can be removed in future
void print_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Memory usage: %ld KB\n", usage.ru_maxrss);
}

void render_screen(Display display, Node* nodes, int numNodes, Road* roads, 
        int numRoads, char* fpsText) {
    // fill screen
    SDL_SetRenderDrawColor(display.renderer, BG.r, BG.g, BG.b, BG.a);
    SDL_RenderClear(display.renderer);

    // render the roads
    SDL_SetRenderDrawColor(display.renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    for (int i=0; i<numRoads; i++) {
        draw_road(display.renderer, *display.camera, roads[i]);
    }

    // render the nodes
    if (display.showNodes) {
        SDL_SetRenderDrawColor(display.renderer, RED.r, RED.g, RED.b, RED.a);
        for (int i=0; i<numNodes; i++) {
            draw_point(display.renderer, *display.camera, nodes[i].pos);
        }
    }

    // render FPS count
    render_fps(display.renderer, fpsText);
}

Node* get_closest_node(Pos pos, Node* nodes, int numNodes) {
    double min_dist = INFINITY;
    Node* min_node = nodes;
    for (int i=0; i<numNodes; i++) {
        double dist = pow(nodes[i].pos.x - pos.x, 2) + pow(nodes[i].pos.y - pos.y, 2);
        if (dist < min_dist) {
            min_dist = dist;
            min_node = nodes + i;
        }
    }
    return min_node;
}

// concatenates the two strings
// result should be freed after use
char* join_string(const char* str1, const char* str2) {
    int len = strlen(str1) + strlen(str2) + 1;
    char* result = malloc(len);

    snprintf(result, len, "%s%s", str1, str2);
    return result;
}

int main(int argc, char** argv) {
    // process arguments
    if (argc < 2) {
        printf("Must provide the file to be loaded. See README.\n");
        exit(1);
    }

    int width = WIDTH;
    int height = HEIGHT;
    int algorithm = ALG_DIJKSTRA;  // the pathfinding algorithm to be used
    for (int i=2; i<argc; i+=2) {
        if (strcmp(argv[i], "-w") == 0) {
            width = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "-h") == 0) {
            height = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--alg") == 0) {
            if (strcmp(argv[i+1], "astar") == 0) {
                algorithm = ALG_A_STAR;
            } else if (strcmp(argv[i+1], "dijkstra") == 0) {
                algorithm = ALG_DIJKSTRA;
            }
        }
    }

    // load in the nodes array
    Node* nodes;
    char* file = join_string(argv[1], ".nodes");
    int numNodes = load_nodes(file, &nodes);

    // load the roads array
    Road* roads;
    file = join_string(argv[1], ".roads");
    int numRoads = load_roads(file, &roads);

    // load the edges array
    Edge* edges;
    file = join_string(argv[1], ".edges");
    load_edges(file, nodes, roads, &edges);

    // initialise SDL
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // setup window
    SDL_Window* window = SDL_CreateWindow("Pathfinder", width, height, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // setup fonts
    TTF_Font* font = TTF_OpenFont("assets/fonts/DejaVuSans.ttf", FONT_SIZE);
    SDL_Texture* message_start = create_message(renderer, font, MSG_START);
    SDL_Texture* message_end = create_message(renderer, font, MSG_END);
    SDL_Texture* message_hold = create_message(renderer, font, MSG_HOLD);

    // initialise the camera
    Camera camera = (Camera){0, 0, 1};

    // create the display struct
    Display display = (Display){renderer, &camera, false, true};

    // keep track of FPS count
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = now;
    double deltaTime = 0;
    int fps = 0;
    double fpsTimer = 0;
    int frameCounter = 0;
    char fpsText[16];

    // important nodes
    Node* focus_node = nodes;
    Node* start_node;
    Node* end_node;

    SDL_Event event;
    bool mouseDown = false; // keep track of mouse state
    bool mouseMotion = false;
    
    // the initial state of the application
    int state = STATE_SELECT_START;

    // animation stuff
    EventList events;
    init_event_list(&events);
    int animation_index = 0;

    // fancy shortest path rendering
    SDL_Texture* glow_tex = create_glow_texture(renderer);

    bool run = true;
    while (run) {
        while (SDL_PollEvent(&event)) {
            // check for quit events
            if (event.type == SDL_EVENT_QUIT) run = false;

            // check for key presses
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_Q) { // q key quits
                    run = false;
                }
                if (event.key.key == SDLK_N) { // n toggles node rendering
                    printf("Node rendering has been toggeled\n");
                    display.showNodes = !display.showNodes;
                }
                if (event.key.key == SDLK_M) { // m to print memory usage
                    print_memory_usage();
                }
                if (event.key.key == SDLK_L) { // general log
                    printf("LOG: %f\n", camera.zoom);
                }
                if (event.key.key == SDLK_SPACE) {
                    if (state == STATE_HOLD) {
                        // run the pathfinder

                        if (algorithm == ALG_DIJKSTRA) {
                            printf("Running Dijkstra's Algorithm\n");
                            dijkstra(nodes, numNodes, start_node, end_node, &events);
                        } else if (algorithm == ALG_A_STAR) {
                            printf("Running A* Algorithm\n");
                            a_star(nodes, numNodes, start_node, end_node, &events);
                        }

                        state = STATE_ANIMATE;
                    }
                }
            }

            // check for mouse clicks
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                mouseDown = true;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (!mouseMotion) { // this is a standard "click" event
                    if (state == STATE_SELECT_START) {
                        start_node = focus_node;
                        state = STATE_SELECT_END;
                        printf("Start node has been selected\n");
                    } else if (state == STATE_SELECT_END) {
                        end_node = focus_node;
                        state = STATE_HOLD;
                        printf("End node has been selected\n");
                    }
                }
                mouseDown = false;
                mouseMotion = false;
            }

            // check for mouse motion
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (mouseDown) {
                    mouseMotion = true;
                    camera.x -= event.motion.xrel / camera.zoom;
                    camera.y -= event.motion.yrel / camera.zoom;
                } else {
                    Pos mouse = get_global(camera, event.motion.x, event.motion.y);
                    focus_node = get_closest_node(mouse, nodes, numNodes);
                }
            }
            else if (event.type == SDL_EVENT_WINDOW_MOVED) {
                mouseMotion = true;
            }

            // check for mouse wheel events
            else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                if (event.wheel.y == 0) continue;

                float mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                int direction = 1;
                if (event.wheel.y > 0) { // zoom in
                    camera.zoom *= ZOOM_FACTOR;
                } else if (event.wheel.y != 0) { // zoom out
                    direction = -1;
                    camera.zoom /= ZOOM_FACTOR;
                }

                camera.x += mouseX / camera.zoom * (ZOOM_FACTOR-1) * direction;
                camera.y += mouseY / camera.zoom * (ZOOM_FACTOR-1) * direction;
            }
        }

        // record elapsed time
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)(now - last) / SDL_GetPerformanceFrequency();
        last = now;

        fpsTimer += deltaTime;
        frameCounter++;

        // only update FPS after given elapsed time
        if (fpsTimer >= 0.2) {
            fps = frameCounter / fpsTimer;
            snprintf(fpsText, sizeof(fpsText), "FPS: %d", fps);

            fpsTimer = 0;
            frameCounter = 0;
        }

        // render the main screen
        render_screen(display, nodes, numNodes, roads, numRoads, fpsText);

        // mode specific rendering
        if (state == STATE_SELECT_START) {
            SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
            render_message(display.renderer, message_start);

            paint_neighbours(renderer, camera, focus_node, 8);
            draw_circle(renderer, camera, focus_node->pos, 4);
        } else if (state == STATE_SELECT_END) {
            SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
            paint_neighbours(renderer, camera, start_node, 8);
            draw_circle(renderer, camera, start_node->pos, 4);


            SDL_SetRenderDrawColor(renderer, PURPLE.r, PURPLE.g, PURPLE.b, PURPLE.a);
            render_message(display.renderer, message_end);

            paint_neighbours(renderer, camera, focus_node, 8);
            draw_circle(renderer, camera, focus_node->pos, 4);
        } else if (state == STATE_HOLD) {
            SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
            paint_neighbours(renderer, camera, start_node, 8);
            draw_circle(renderer, camera, start_node->pos, 4);


            SDL_SetRenderDrawColor(renderer, PURPLE.r, PURPLE.g, PURPLE.b, PURPLE.a);
            paint_neighbours(renderer, camera, end_node, 8);
            draw_circle(renderer, camera, end_node->pos, 4);

            SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
            render_message(display.renderer, message_hold);
        } else if (state == STATE_ANIMATE) {
            SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
            draw_circle(renderer, camera, start_node->pos, 4);

            SDL_SetRenderDrawColor(renderer, PURPLE.r, PURPLE.g, PURPLE.b, PURPLE.a);
            draw_circle(renderer, camera, end_node->pos, 4);

            // add highlighted edges
            float t;
            for (int i = 0; i < animation_index && i < events.size; i++) {
                t = (float)i / events.size;
                SDL_SetRenderDrawColor(
                        renderer, 
                        GRAD_START[0] + t * (GRAD_END[0] - GRAD_START[0]),
                        GRAD_START[1] + t * (GRAD_END[1] - GRAD_START[1]),
                        GRAD_START[2] + t * (GRAD_END[2] - GRAD_START[2]),
                        0xff
                );
                draw_road(renderer, camera, *events.data[i].edge->road);
            }

            if (animation_index >= events.size) {
                paint_path(renderer, camera, end_node, glow_tex);
            } else {
                animation_index += 60; // can change animation speed here
            }

        }

        // update the display
        SDL_RenderPresent(display.renderer);
    }
}
