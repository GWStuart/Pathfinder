#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "loadData.h"
#include "config.h"
#include "renderUtils.h"

// can remove this include later
#include <sys/resource.h>

// there are a few states in which the software can be.
// STATE_SELECT_START --> select pathfinder start point
// STATE_SELECT_END   --> select pathfinder end point
// STATE_PATHFIND     --> running the pathfinder
typedef enum {
    STATE_SELECT_START = 1,
    STATE_SELECT_EEND = 2,
    STATE_PATHFIND = 3
} AppState;

// define string constants
const char* const MSG_START = "Select a starting point";

// temporary function used to check what the memory usage is.
// this function can be removed in future
void print_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Memory usage: %ld KB\n", usage.ru_maxrss);
}

void render_screen(Display display, Node* nodes, int numNodes, Road* roads, 
        int numRoads, SDL_Texture* message, char* fpsText) {
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
        SDL_SetRenderDrawColor(display.renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
        for (int i=0; i<numNodes; i++) {
            draw_point(display.renderer, *display.camera, nodes[i].pos);
        }
    }

    // render FPS count
    render_fps(display.renderer, fpsText);

    // render the message
    render_message(display.renderer, message);

    // update the display
    SDL_RenderPresent(display.renderer);
}

int main() {
    // load in the nodes array
    Node* nodes;
    int numNodes = load_nodes("assets/data/Brisbane.nodes", &nodes);

    // load the roads array
    Road* roads;
    int numRoads = load_roads("assets/data/Brisbane.roads", nodes, &roads);

    // initialise SDL
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // setup window
    SDL_Window* window = SDL_CreateWindow("Pathfinder", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // setup fonts
    TTF_Font* font = TTF_OpenFont("assets/fonts/DejaVuSans.ttf", FONT_SIZE);
    SDL_Texture* message = create_message(renderer, font, MSG_START);

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

    SDL_Event event;
    bool mouseDown = false; // keep track of mouse state
    
    // the initial state of the application
    int state = STATE_SELECT_START;

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
                    display.showNodes = !display.showNodes;
                }
                if (event.key.key == SDLK_M) { // n toggles node rendering
                    print_memory_usage();
                }
            }

            // check for mouse clicks
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                mouseDown = true;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                mouseDown = false;
            }

            // check for mouse motion
            else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (mouseDown) {
                    camera.x -= event.motion.xrel / camera.zoom;
                    camera.y -= event.motion.yrel / camera.zoom;
                } else if (state <= 2) {
                    printf("hello\n");
                }
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

        render_screen(display, nodes, numNodes, roads, numRoads, message, 
                fpsText);
    }
}
