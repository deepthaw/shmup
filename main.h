#include <SDL.h>
#include <tmx.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCALE 3
#define SCREEN_FPS 60
#define SCREEN_TICKS_PER_FRAME 1000 / SCREEN_FPS
#define NUM_BG_TILES 3
#define TILE_X 16
#define TILE_Y 16
#define SCROLL_SPEED 1
#define SCROLL_STEP SCREEN_WIDTH 

typedef struct gameData {
  int x, y;
} gameData;

typedef struct renderList {
  SDL_Texture *screen[2];
  SDL_Texture *buffer;
  tmx_layer* layer;
  struct renderList *next;
} renderList;

extern SDL_Renderer* gRenderer;
