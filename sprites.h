#include <glib.h>
#include <SDL.h>
#include <SDL_image.h>

typedef struct spriteTex {
  SDL_Texture *tex;
  SDL_Rect rect;
} spriteTex;

typedef struct Sprite {
  SDL_Texture *tex;
  spriteTex **frames;
  int posx, posy;
  SDL_Rect hitbox;
  SDL_Rect rect;
} Sprite;

typedef struct spritelist {
  struct Sprite *sprite;
  struct spritelist *next;
} spritelist;

extern SDL_Renderer *gRenderer;

enum spriteState {
  SPNEUTRAL,
  SPFORWARD,
  SPREVERSE,
  SPTILTUP,
  SPFULLUP,
  SPTILTDN,
  SPFULLDN,
  NUMSTATES
};

extern const char *stateTable[NUMSTATES];
