#include <glib.h>
#include <SDL.h>
#include <SDL_image.h>
#include "tmx_render.h"

extern SDL_Renderer *gRenderer;
extern tmx_map *map; 

typedef struct spriteTex {
  SDL_Texture *tex;
  SDL_Rect rect;
} spriteTex;

typedef struct frameList {
  uint32_t tics;
  spriteTex *frame;
  struct frameList *next;
} frameList;

typedef struct Sprite {
  frameList **frames;
  SDL_Rect hitbox;
  SDL_Rect rect;
} Sprite;

typedef struct spritelist {
  struct Sprite *sprite;
  struct spritelist *next;
} spritelist;

enum spriteFrame {
  SPNEUTRAL,
  SPFORWARD,
  SPREVERSE,
  SPTILTUP,
  SPFULLUP,
  SPTILTDN,
  SPFULLDN,
  NUMFRAMES
};

enum spriteType {
  PLAYER,
  ENEMY
};

enum spriteNames {
  P_VIPER,
  E_ROLLER,
  NUMSPRITENAMES
};

extern const char *frameTable[NUMFRAMES];
extern const char *spriteNames[NUMSPRITENAMES];

Sprite **loadSpritesFromDisk(const char *spriteNames[], int count);

