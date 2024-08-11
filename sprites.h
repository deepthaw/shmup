/* Sprites are the base graphic data that actors use
 * when they are instantiated. A frame is an individual
 * frame of animation, while a frameList is the set of
 * frames that will be drawn. All frameLists are circular
 * linked lists so that they animated in a loop indefinitely.
 *
*/

#include <glib.h>
#include <SDL.h>
#include <SDL_image.h>
#include "tmx_render.h"

extern SDL_Renderer *gRenderer;
extern tmx_map *map; 

typedef struct Frame {
  SDL_Texture *tex;
  SDL_Rect rect;
} Frame;

typedef struct Animation {
  uint32_t tics;
  Frame *frame;
  struct Animation *next;
} Animation;

typedef struct Sprite {
  Animation **animation;
  SDL_Rect hitbox;
  SDL_Rect rect;
} Sprite;

enum Animations {
  SPNEUTRAL,
  SPFORWARD,
  SPREVERSE,
  SPTILTUP,
  SPFULLUP,
  SPTILTDN,
  SPFULLDN,
  HITBOX,
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
