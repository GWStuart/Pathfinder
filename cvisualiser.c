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
#define ZOOM_FACTOR 1.01
#define ZOOM_FACTOR_MOUSE 1.1

// define SDL colours
SDL_Color BG = {0x28, 0x28, 0x28, 0xff};
SDL_Color WHITE = {0xff, 0xff, 0xff, 0xff};

typedef struct {
    int x;
    int y;
    double zoom;
} Camera;

void draw_circle(SDL_Renderer* renderer, Pos pos, int r) {
    // rect method (FPS 12xx)
    //SDL_FRect rect = (SDL_FRect){pos.x*600 - r, pos.y*600 - r, r*2, r*2};
    //SDL_RenderFillRect(renderer, &rect);

    // distance from origin method (FPS 5XX to 6XX)
//    int px = pos.x * 600;
//    int py = pos.y * 600;
//    int r2 = pow(r, 2);
//    for (int x = px - r; x <= px + r; x++) {
//        for (int y = py - r; y <= py + r; y++) {
//            if (pow(x - px, 2) + pow(y - py, 2) < r2) {
//                SDL_RenderPoint(renderer, x, y);
//            }
//        }
//    }

    // vertical line method
    int px = pos.x * 600;
    int py = pos.y * 600;
    int r2 = pow(r, 2);
    for (int x = px - r; x <= px + r; x++) {
        int dy = sqrt(r2 - pow(x - px, 2));
        SDL_RenderLine(renderer, x, py - dy, x, py + dy);
    }

}

void render_screen(SDL_Renderer* renderer, Node* nodes, int numNodes, 
        char* fpsText, TTF_Font* font) {
    // fill screen
    SDL_SetRenderDrawColor(renderer, BG.r, BG.g, BG.b, BG.a);
    SDL_RenderClear(renderer);

    // render the nodes
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    Node node;
    for (int i=0; i<numNodes; i++) {
        node = nodes[i];
        draw_circle(renderer, node.pos, 5);
    }

    // render FPS count
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, fpsText, 
            strlen(fpsText), WHITE);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, 
            textSurface);
    SDL_FRect textRect = {10, 10, textSurface->w, textSurface->h};
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);

    // update the display
    SDL_RenderPresent(renderer);
}

int main() {
    // load in the nodes array
    Node* nodes;
    int numNodes = load_nodes("data/no.nodes", &nodes);
    //int numNodes = load_nodes("data/RomeFull.nodes", &nodes);

    // initialise the SDL window
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("Pathfinder", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    //SDL_SetWindowFullscreen(window, true); // uncomment for fullscreen

    // setup fonts
    TTF_Font* font = TTF_OpenFont("assets/fonts/DejaVuSans.ttf", 20);

    // keep track of FPS count
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = now;
    double deltaTime = 0;
    double fps = 0;
    double fpsTimer = 0;
    int frameCounter = 0;
    char fpsText[16];

    SDL_Event event;

    bool run = true;
    while (run) {
        while (SDL_PollEvent(&event)) {
            // check for quit events
            if (event.type == SDL_EVENT_QUIT) run = false;

            // check for key presses
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_Q) run = false; // q key quits
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
            snprintf(fpsText, sizeof(fpsText), "FPS: %.2f", fps);

            fpsTimer = 0;
            frameCounter = 0;
        }

        render_screen(renderer, nodes, numNodes, fpsText, font);
    }
}
