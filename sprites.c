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

Sprite *createSprite(char *name, int type) {
  printf("Trying to create %s\n", name);
  Sprite *s = NULL;
  s = malloc(sizeof(Sprite));
  tmx_tileset *tileset = tmx_find_tileset_by_name(map, name)->tileset;
  int tilecount = tileset->tilecount;
  tmx_tile *tiles = tileset->tiles;
  s->animtex = malloc(NUMSTATES * sizeof(animateLoop *));
  for (int i = 0; i != NUMSTATES; i++) {
    const char *currentstate = stateTable[i];
    char *n = calloc(strlen(currentstate) + 1, sizeof(char));
    s->animtex[i] = NULL;
    strcpy(n, currentstate);
    for (int j = 0; j != 10; j++) {
      animateLoop *current;
      n[strlen(currentstate)] = j + '0';
      for (int k = 0; k != tilecount; k++) {
        tmx_property prop = *tmx_get_property(tiles[k].properties, "state");
        if (strcmp(prop.value.string, n) == 0) {
          animateLoop *new = malloc(sizeof(animateLoop));
          new->frame = malloc(sizeof(spriteTex));
          new->frame->tex = IMG_LoadTexture(gRenderer, tiles[k].image->source);
          new->frame->rect.h = tiles[k].height;
          new->frame->rect.w = tiles[k].width;
          new->tics = 5;
          new->next = NULL;

          if (s->animtex[i] == NULL) {
            s->animtex[i] = new;
          } else {
            current = s->animtex[i];
            while (current->next != NULL) {
              current = current->next;
            }
            current->next = new;
          }
        }
      }
    }
    if (s->animtex[i] != NULL) {
      animateLoop *tmp = s->animtex[i];
      while (tmp->next != NULL) {
        tmp = tmp->next;
      }
      tmp->next = s->animtex[i];
    }
  }
  s->type = type;
  setSpriteState(s, SPNEUTRAL);
  return s;
}

void addSprite(spritelist **sprites, Sprite *s) {
  spritelist *new = calloc(1, sizeof(spritelist));
  if (new == NULL) {
    printf("failed to allocate memory!\n");
    exit(EXIT_FAILURE);
  }
  new->sprite = s;
  new->next = NULL;
  if (*sprites == NULL) {
    *sprites = new;
  } else {
    spritelist *node = *sprites;
    while (node->next != NULL) {
      node = node->next;
    }
    node->next = new;
  }
}

void updateSprite(Sprite *s) {
  int state = s->state;
 
  if (s->elapsed++ > s->animtex[state]->tics) {
  s->animtex[state] = s->animtex[state]->next;
  s->tex = s->animtex[state]->frame->tex;
  s->elapsed = 0;
  }
}

void setSpriteState(Sprite *sprite, int state) {
    sprite->state = state;
    sprite->elapsed = 0;
    sprite->tex = sprite->animtex[state]->frame->tex;
    sprite->rect.h = sprite->animtex[state]->frame->rect.h;
    sprite->rect.w = sprite->animtex[state]->frame->rect.w;
    printf("sprite at %d state set to %d %d\n", sprite, state, sprite->state);
}
