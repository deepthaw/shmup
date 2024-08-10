#include "sprites.h"
#include <SDL_image.h>
#include <SDL_render.h>

const char *frameTable[NUMFRAMES] = {"SPNEUTRAL", "SPFORWARD", "SPREVERSE",
                                     "SPTILTUP",  "SPFULLUP",  "SPTILTDN",
                                     "SPFULLDN"};

const char *spriteNames[NUMSPRITENAMES] = {"P_VIPER", "E_ROLLER"};

spriteTex *loadSpriteFromDisk(const char *filename) {
  spriteTex *t = malloc(sizeof(spriteTex));
  t->tex = IMG_LoadTexture(gRenderer, filename);
  if (t->tex != NULL) {
    SDL_QueryTexture(t->tex, NULL, NULL, &t->rect.w, &t->rect.h);
    t->rect.x = t->rect.y = 0;
    return t;
  }
  return NULL;
}

void makeFrameListLoop(frameList *frames) {
  if (frames == NULL) {
    return; // Empty list, nothing to do
  }
  frameList *head = frames;
  frameList *node = frames;

  while (node->next != NULL) {
    node = node->next;
  }
  node->next = head;
}

frameList **addFrame(frameList **frames, spriteTex *t) {
  frameList *new = calloc(1, sizeof(frameList));
  if (new == NULL) {
    printf("failed to allocate memory!\n");
    exit(EXIT_FAILURE);
  }
  new->frame = t;
  new->tics = 5;
  new->next = NULL;
  if (*frames == NULL) {
    *frames = new;
  } else {
    frameList *node = *frames;
    while (node->next != NULL) {
      node = node->next;
    }
    node->next = new;
  }
  return frames;
}

Sprite **loadSpritesFromDisk(const char *spriteNames[], int count) {
  // Allocate memory for the array of sprite pointers
  Sprite **spriteData = calloc(count, sizeof(Sprite *));

  // Loop through each sprite name
  for (int name = 0; name < count; name++) {
    // Allocate memory for each individual Sprite
    spriteData[name] = calloc(1, sizeof(Sprite));
    Sprite *sname = spriteData[name];

    // Allocate memory for the frames array in the Sprite
    sname->frames = calloc(NUMFRAMES, sizeof(frameList *));

    // Loop through each frame in the sprite
    for (int frame = 0; frame < NUMFRAMES; frame++) {
      sname->frames[frame] = NULL; // Initialize the frame list pointer

      // Loop through each texture (up to 10 per frame)
      for (int tex = 0; tex < 10; tex++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "./SPRITES/%s/%s%u.png",
                 spriteNames[name], frameTable[frame], tex);

        spriteTex *t = loadSpriteFromDisk(filename);

        if (t != NULL) {
          printf("loaded %s from disk\n", filename);
          sname->frames[frame] = *addFrame(&(sname->frames[frame]), t);
        }
      }

      // Ensure the frame list is looped (a circular list)
      makeFrameListLoop(sname->frames[frame]);
    }
  }

  return spriteData;
}
