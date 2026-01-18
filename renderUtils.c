#include "config.h"
#include <SDL3_ttf/SDL_ttf.h>

/* ###############################################
 *
 * A collection of some useful rendering functions
 *
 * ###############################################
*/


SDL_Point get_local(Camera camera, Pos pos) {
    return (SDL_Point){(pos.x*INIT_SCALE - camera.x) * camera.zoom, 
                       (pos.y*INIT_SCALE - camera.y) * camera.zoom};
}

SDL_FPoint get_local_float(Camera camera, Pos pos) {
    return (SDL_FPoint){(pos.x*INIT_SCALE - camera.x) * camera.zoom, 
                       (pos.y*INIT_SCALE - camera.y) * camera.zoom};
}

Pos get_global(Camera camera, float x, float y) {
    return (Pos){
        .x = (x / camera.zoom + camera.x) / INIT_SCALE,
        .y = (y / camera.zoom + camera.y) / INIT_SCALE
    };
}

// Efficiently renders a small point to the given position
void draw_point(SDL_Renderer* renderer, Camera camera, Pos pos) {
    SDL_Point local = get_local(camera, pos);

    // draw the points
    SDL_FRect rect;
    if (camera.zoom <= 1) {
        rect = (SDL_FRect){local.x - 1, local.y - 1, 2, 2};
    } else if (camera.zoom >= 10) {
        rect = (SDL_FRect){local.x - 4, local.y - 4, 8, 8};
    } else {
        double sx = camera.zoom*(1.0/3.0) + 2.0/3.0;
        double sx2 = 2 * sx;
        rect = (SDL_FRect){
            local.x - sx, 
            local.y - sx, 
            sx2, 
            sx2
        };
    }

    SDL_RenderFillRect(renderer, &rect);
}

// Efficiently renders a circle centered at the given position and with a radius
// that is proportional to the current camera zoom
void draw_circle(SDL_Renderer* renderer, Camera camera, Pos pos, int r) {
    SDL_Point local = get_local(camera, pos);
    SDL_FRect rect = (SDL_FRect){local.x - 5, local.y - 5, 10, 10};
    SDL_RenderFillRect(renderer, &rect);
}

// Efficiently renders a road from a given road struct
void draw_road(SDL_Renderer* renderer, Camera camera, Road road) {
    SDL_FPoint points[road.pathCount];
    for (int i=0; i<road.pathCount; i++) {
        points[i] = get_local_float(camera, road.path[i]);
    }
    SDL_RenderLines(renderer, points, road.pathCount);
}

// Creates a new message texture
SDL_Texture* create_message(SDL_Renderer* renderer, TTF_Font* font, 
        const char* message) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, message, 
            strlen(message), BG);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return texture;
}

// Renders a given message texture to the message region of the screen in the
// lower left corner.
void render_message(SDL_Renderer* renderer, SDL_Texture* message) {
    SDL_FRect rect = (SDL_FRect){0, HEIGHT - message->h, message->w + PADDING*2, 
                       message->h};
    SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
    SDL_RenderFillRect(renderer, &rect);
    rect = (SDL_FRect){PADDING, HEIGHT - message->h, message->w, message->h};
    SDL_RenderTexture(renderer, message, NULL, &rect);
}

// Renders the provided fps count to the upper left corner of the screen
void render_fps(SDL_Renderer* renderer, char* fpsText) {
    SDL_SetRenderDrawColor(renderer, BG.r, BG.g, BG.b, BG.a);
    SDL_FRect rect = (SDL_FRect){0, 0, 70, 15};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderDebugText(renderer, 5, 5, fpsText);
}


