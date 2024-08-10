#include "actors.h"

void setActorFrame(Actor *a, int frame) {
  a->frame = frame;
  a->elapsed = 0;
  a->currentTex = a->frames[frame]->frame->tex;
  a->rect.h = a->frames[frame]->frame->rect.h;
  a->rect.w = a->frames[frame]->frame->rect.w;
}

Actor *createActor(Sprite **spritedata, const char *name, int type, int state) {
    // Find the sprite corresponding to the name
    Sprite *source = NULL;
    for (int i = 0; i < NUMSPRITENAMES; i++) {
        if (strcmp(spriteNames[i], name) == 0) {
            source = spritedata[i];
            break;  // Exit the loop early when the sprite is found
        }
    }
    
    // If the source sprite is not found, return NULL
    if (source == NULL) {
        return NULL;
    }
    
    // Allocate memory for the new actor
    Actor *a = malloc(sizeof(Actor));
    if (a == NULL) {
        printf("Failed to allocate memory for Actor!\n");
        return NULL;
    }

    // Point to the original sprite data without allocating unnecessary memory
    a->frames = source->frames;
    a->rect = source->rect;
    a->hitbox = source->hitbox;
    a->type = type;
    a->state = state;  // Use the passed state instead of hardcoding 0
    a->elapsed = 0;

    // Initialize the actor's frame
    setActorFrame(a, SPNEUTRAL);
    
    return a;
}

void updateActorFrame(Actor *a) {
  int frame = a->frame;

  if (a->elapsed++ > a->frames[frame]->tics) {
    a->frames[frame] = a->frames[frame]->next;
    a->currentTex = a->frames[frame]->frame->tex;
    a->elapsed = 0;
  }
}

void addActor(actorlist **actors, Actor *a) {
  actorlist *new = calloc(1, sizeof(actorlist));
  if (new == NULL) {
    printf("failed to allocate memory!\n");
    exit(EXIT_FAILURE);
  }
  new->actor = a;
  new->next = NULL;
  if (*actors == NULL) {
    *actors = new;
  } else {
    actorlist *node = *actors;
    while (node->next != NULL) {
      node = node->next;
    }
    node->next = new;
  }
}
