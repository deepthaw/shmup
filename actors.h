#include "sprites.h"

typedef struct Actor {
  spriteTex *currentframe;
  SDL_Texture *currentTex;
  frameList **frames;
  int elapsed; 
  SDL_Rect hitbox;
  SDL_Rect rect;
  int type;
  int state;
  int frame;
} Actor;

typedef struct actorlist {
  struct Actor *actor;
  struct actorlist *next;
} actorlist;

Actor *createActor(Sprite **spritedata, const char* name, int type, int state);
void setActorFrame(Actor *a, int frame); 
void updateActorFrame(Actor *a);
void addActor(actorlist **Actors, Actor *a);

