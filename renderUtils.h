#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include "config.h"

// convert a global coordinate to a local coordinate
SDL_Point get_local(Camera camera, Pos pos);

// same as function above but returns an FPoint (required by some SDL functions)
SDL_FPoint get_local_float(Camera camera, Pos pos);

/* draw_point()
 * function used to draw small points on the screen at the specified position
 * renderer: the renderer to use
 * pos: the position struct at which to render the point
 */
void draw_point(SDL_Renderer* renderer, Camera camera, Pos pos);

/* draw_road()
 * function used to draw the given road
 * renderer: the renderer to use
 * pos: the position struct at which to render the point
 */
void draw_road(SDL_Renderer* renderer, Camera camera, Road road);

/* create_message()
 * creates an SDL_Texture of the specfied message using the given font
 */
SDL_Texture* create_message(SDL_Renderer* renderer, TTF_Font* font, 
        const char* message);

/* render_message()
 * renders thee given message texture to the bottom left of the screen
 */
void render_message(SDL_Renderer* renderer, SDL_Texture* message);

/* render_fps()
 * a helpful utility  function to render the current fps in the corner as
 * provided by the fpsText string
 */
void render_fps(SDL_Renderer* renderer, char* fpsText);

#endif
