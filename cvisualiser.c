#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "loadData.h"

// define screen sizes
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

// define string constants
const char* const MSG_START = "Select a starting point";

// define SDL colours
SDL_Color BG = {0x28, 0x28, 0x28, 0xff};
SDL_Color WHITE = {0xff, 0xff, 0xff, 0xff};
SDL_Color RED = {0xff, 0x00, 0x00, 0xff};
SDL_Color BLUE = {0x70, 0xd6, 0xff, 0xff};

typedef struct {
    double x;
    double y;
    double zoom;
} Camera;

typedef struct {
    SDL_Renderer* renderer;
    Camera* camera;
    bool showNodes;
    bool showHighlihts;
} Display;

typedef struct {
    Node start;
    Node end;
} Pathfinder;

// convert a global coordinate to a local coordinate
SDL_Point get_local(Camera camera, Pos pos) {
    return (SDL_Point){(pos.x*INIT_SCALE - camera.x) * camera.zoom, 
                       (pos.y*INIT_SCALE - camera.y) * camera.zoom};
}

// convert a global coordinate to a local coordinate
SDL_FPoint get_local_float(Camera camera, Pos pos) {
    return (SDL_FPoint){(pos.x*INIT_SCALE - camera.x) * camera.zoom, 
                       (pos.y*INIT_SCALE - camera.y) * camera.zoom};
}

/* draw_point()
 * function used to draw small points on the screen at the specified position
 * renderer: the renderer to use
 * pos: the position struct at which to render the point
 */
void draw_point(SDL_Renderer* renderer, Camera camera, Pos pos) {
    SDL_Point local = get_local(camera, pos);
    SDL_FRect rect = (SDL_FRect){local.x - 1, local.y - 1, 2, 2};

    SDL_RenderFillRect(renderer, &rect);
}

/* draw_road()
 * function used to draw the given road
 * renderer: the renderer to use
 * pos: the position struct at which to render the point
 */
void draw_road(SDL_Renderer* renderer, Camera camera, Road road) {
    SDL_FPoint points[road.pathCount];
    for (int i=0; i<road.pathCount; i++) {
        points[i] = get_local_float(camera, road.path[i]);
    }
    SDL_RenderLines(renderer, points, road.pathCount);
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
    SDL_SetRenderDrawColor(display.renderer, BG.r, BG.g, BG.b, BG.a);
    SDL_FRect rect = (SDL_FRect){0, 0, 70, 15};
    SDL_RenderFillRect(display.renderer, &rect);
    SDL_SetRenderDrawColor(display.renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderDebugText(display.renderer, 5, 5, fpsText);

    // render the message
    rect = (SDL_FRect){0, HEIGHT - message->h, message->w + PADDING*2, 
                       message->h};
    SDL_SetRenderDrawColor(display.renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
    SDL_RenderFillRect(display.renderer, &rect);
    rect = (SDL_FRect){PADDING, HEIGHT - message->h, message->w, message->h};
    SDL_RenderTexture(display.renderer, message, NULL, &rect);

    // update the display
    SDL_RenderPresent(display.renderer);
}

SDL_Texture* create_message(SDL_Renderer* renderer, TTF_Font* font, 
        const char* message) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, message, 
            strlen(message), BG);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return texture;
}

int main() {
    // load in the nodes array
    Node* nodes;
    //int numNodes = load_nodes("assets/data/no.nodes", &nodes);
    //int numNodes = load_nodes("assets/data/RomeFull.nodes", &nodes);
    int numNodes = load_nodes("assets/data/Brisbane.nodes", &nodes);

    // load the roads array
    Road* roads;
    //int numRoads = load_roads("assets/data/RomeFull.roads", nodes, &roads);
    int numRoads = load_roads("assets/data/Brisbane.roads", nodes, &roads);

    // initialise SDL
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // setup window
    SDL_Window* window = SDL_CreateWindow("Pathfinder", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    //SDL_SetWindowFullscreen(window, true); // uncomment for fullscreen

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
            }

            // check for mouse clicks
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                mouseDown = true;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                mouseDown = false;
            }

            // check for mouse motion
            else if (event.type == SDL_EVENT_MOUSE_MOTION && mouseDown) {
                camera.x -= event.motion.xrel / camera.zoom;
                camera.y -= event.motion.yrel / camera.zoom;
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
