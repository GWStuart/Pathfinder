#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WIDTH 800
#define HEIGHT 600

void render_screen(SDL_Renderer* renderer) {
    // fill screen
    SDL_SetRenderDrawColor(renderer, 0x28, 0x28, 0x28, 0xff);
    SDL_RenderClear(renderer);

    // update the display
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Pathfinder", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

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

        render_screen(renderer);
    }
}
