#include "sprites.h"
#include <SDL_image.h>

const char *stateTable[NUMSTATES] = {
  "SPNEUTRAL",
  "SPNEUTRAL1",
  "SPNEUTRAL2",
  "SPFORWARD",
  "SPREVERSE",
  "SPTILTUP",
  "SPFULLUP",
  "SPTILTDN",
  "SPFULLDN" };

void setSpriteState(Sprite *sprite, int state) {
  if (sprite->frames[state] != NULL) {
    sprite->tex = sprite->frames[state]->tex;
    sprite->rect.h = sprite->frames[state]->rect.h;
    sprite->rect.w = sprite->frames[state]->rect.w;
  }
}
