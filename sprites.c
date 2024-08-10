#include "sprites.h"
#include <SDL_image.h>
#include <SDL_render.h>

const char *frameTable[NUMFRAMES] = {"SPNEUTRAL", "SPFORWARD", "SPREVERSE",
                                     "SPTILTUP",  "SPFULLUP",  "SPTILTDN",
                                     "SPFULLDN"};

const char *spriteNames[NUMSPRITENAMES] = {"P_VIPER", "E_ROLLER"};

Frame *loadSpriteFromDisk(const char *filename) {
  Frame *t = malloc(sizeof(Frame));
  t->tex = IMG_LoadTexture(gRenderer, filename);
  if (t->tex != NULL) {
    SDL_QueryTexture(t->tex, NULL, NULL, &t->rect.w, &t->rect.h);
    t->rect.x = t->rect.y = 0;
    return t;
  }
  return NULL;
}

void makeAnimationLoop(Animation *animation) {
  if (animation == NULL) {
    return; // Empty list, nothing to do
  }
  Animation *head = animation;
  Animation *node = animation;

  while (node->next != NULL) {
    node = node->next;
  }
  node->next = head;
}

Animation **addFrame(Animation **animation, Frame *t) {
  Animation *new = calloc(1, sizeof(Animation));
  if (new == NULL) {
    printf("failed to allocate memory!\n");
    exit(EXIT_FAILURE);
  }
  new->frame = t;
  new->tics = 5;
  new->next = NULL;
  if (*animation == NULL) {
    *animation = new;
  } else {
    Animation *node = *animation;
    while (node->next != NULL) {
      node = node->next;
    }
    node->next = new;
  }
  return animation;
}

Sprite **loadSpritesFromDisk(const char *spriteNames[], int count) {
  // Allocate memory for the array of sprite pointers
  Sprite **spriteData = calloc(count, sizeof(Sprite *));

  // Loop through each sprite name
  for (int name = 0; name < count; name++) {
    // Allocate memory for each individual Sprite
    spriteData[name] = calloc(1, sizeof(Sprite));
    Sprite *sname = spriteData[name];

    // Allocate memory for the animation array in the Sprite
    sname->animation = calloc(NUMFRAMES, sizeof(Animation *));

    // Loop through each frame in the sprite
    for (int frame = 0; frame < NUMFRAMES; frame++) {
      sname->animation[frame] = NULL; // Initialize the frame list pointer

      // Loop through each texture (up to 10 per frame)
      for (int tex = 0; tex < 10; tex++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "SPRITES/%s/%s%u.png",
                 spriteNames[name], frameTable[frame], tex);

        Frame *t = loadSpriteFromDisk(filename);

        if (t != NULL) {
          printf("loaded %s from disk\n", filename);
          sname->animation[frame] = *addFrame(&(sname->animation[frame]), t);
        }
      }

      // Ensure the frame list is looped (a circular list)
      makeAnimationLoop(sname->animation[frame]);
    }
  }

  return spriteData;
}
