#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include "config.h"
#include <SDL3_ttf/SDL_ttf.h>

// convert a global coordinate to a local coordinate
SDL_Point get_local(Camera camera, Pos pos);

// same as function above but returns an FPoint (required by some SDL functions)
SDL_FPoint get_local_float(Camera camera, Pos pos);

// convert a local coordinate to a global one
Pos get_global(Camera camera, float x, float y);

/* draw_point()
 * function used to draw small points on the screen at the specified position
 * renderer: the renderer to use
 * pos: the position struct at which to render the point
 */
void draw_point(SDL_Renderer* renderer, Camera camera, Pos pos);

/* draw_circle()
 * function used to draw circles on the screen at the specified position
 * renderer: the renderer to use
 * pos: the position struct at which to render the point
 */
void draw_circle(SDL_Renderer* renderer, Camera camera, Pos pos, int r);

/* draw_road()
 * function used to draw the given road
 * renderer: the renderer to use
 * pos: the position struct at which to render the point
 */
void draw_road(SDL_Renderer* renderer, Camera camera, Road road);

// creates a glow texture thaat is used for the shortest path rendering
SDL_Texture* create_glow_texture(SDL_Renderer *renderer);

// thicker version of the draw using the given texture
void draw_road_thick(SDL_Renderer* renderer, Camera camera, Road road, 
        SDL_Texture* tex);

/* create_message()
 * creates an SDL_Texture of the specfied message using the given font
 */
SDL_Texture* create_message(SDL_Renderer* renderer, TTF_Font* font, 
        const char* message);

/* render_message()
 * renders thee given message texture to the bottom left of the screen
 */
void render_message(SDL_Renderer* renderer, SDL_Texture* message, int height);

/* render_fps()
 * a helpful utility  function to render the current fps in the corner as
 * provided by the fpsText string
 */
void render_fps(SDL_Renderer* renderer, char* fpsText);

/* paint_neighbours()
 * function used to highlight all neighbours of the given node. This function
 * is run recursivly to colour neighbours of neighbours up to the given depth.
 *
 * (note that this function could be made more efficient as it does draw over
 * itself however the impact on performance is low enough that I have not made
 * it a priority to fix this).
 */
void paint_neighbours(SDL_Renderer* renderer, Camera camera, Node* node, int depth);

#endif
