#include "tmx.h"
#include <SDL.h>

#define TILE_X 16
#define TILE_Y 16

extern SDL_Renderer *gRenderer;
void* SDL_tex_loader(const char *path);
void draw_sprite(void *image, unsigned int dx, unsigned int dy, float opacity, unsigned int flags); 
void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags);

SDL_Texture* draw_layer(tmx_map *map, tmx_layer *layer, unsigned int x_offset_tiles, unsigned int y_offset_tiles, unsigned int x_size_tiles, unsigned int y_size_tiles);
//void draw_layer(tmx_map *map, tmx_layer *layer);
//void draw_all_layers(tmx_map *map, tmx_layer *layers);
void set_color(int color);
//void render_map(tmx_map *map);





