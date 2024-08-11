#include "tmx_render.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <stdbool.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCALE 3
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000 / SCREEN_FPS
#define SCROLL_SPEED 1
#define SCROLL_STEP SCREEN_WIDTH
#define SCROLL_STEP_TILES SCROLL_STEP / TILE_X

typedef struct renderList {
  SDL_Texture *screen[2];
  SDL_Texture *buffer;
  tmx_layer *layer;
  struct renderList *next;
} renderList;

extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;
extern renderList *buffers;

SDL_Renderer *initializeGraphics();
renderList *initializeRenderList(tmx_map *map);
