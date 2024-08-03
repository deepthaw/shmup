#include <glib.h>
#include <SDL.h>
#include <SDL_image.h>

extern SDL_Renderer *gRenderer;

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


enum spriteState {
  SPNEUTRAL,
  SPNEUTRAL1,
  SPNEUTRAL2,
  SPFORWARD,
  SPREVERSE,
  SPTILTUP,
  SPFULLUP,
  SPTILTDN,
  SPFULLDN,
  NUMSTATES
};

extern const char *stateTable[NUMSTATES];

void setSpriteState(Sprite *, int);


