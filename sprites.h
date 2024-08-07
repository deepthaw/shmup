#include <glib.h>
#include <SDL.h>
#include <SDL_image.h>

extern SDL_Renderer *gRenderer;

typedef struct spriteTex {
  SDL_Texture *tex;
  SDL_Rect rect;
} spriteTex;

typedef struct animateLoop {
  uint32_t tics;
  spriteTex *frame;
  struct animateLoop *next;
} animateLoop;

typedef struct Sprite {
  SDL_Texture *tex;
  animateLoop **animtex;
  int elapsed; 
  int posx, posy;
  SDL_Rect hitbox;
  SDL_Rect rect;
  int type;
  int state;
} Sprite;

typedef struct spritelist {
  struct Sprite *sprite;
  struct spritelist *next;
} spritelist;


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

enum spriteType {
  PLAYER,
  ENEMY
};

extern const char *stateTable[NUMSTATES];

void setSpriteState(Sprite *, int);


