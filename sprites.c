#include "sprites.h"
#include <SDL_image.h>

const char *stateTable[NUMSTATES] = {
  "SPNEUTRAL",
  "SPFORWARD",
  "SPREVERSE",
  "SPTILTUP",
  "SPFULLUP",
  "SPTILTDN",
  "SPFULLDN" };

void setSpriteState(Sprite *sprite, int state) {
    sprite->state = state;
    sprite->elapsed = 0;
    sprite->tex = sprite->animtex[state]->frame->tex;
    sprite->rect.h = sprite->animtex[state]->frame->rect.h;
    sprite->rect.w = sprite->animtex[state]->frame->rect.w;
    printf("sprite at %d state set to %d %d\n", sprite, state, sprite->state);
}
