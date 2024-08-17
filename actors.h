/* Actors are the actual entities that move around the screen
 * they use sprites as their underlying graphical data
 */
#pragma once
#include "sprites.h"

typedef struct Actor {
  Sprite *sprite;
  Animation *currentframe;
  float x,y;
  int elapsed; 
  int type;
  int state;
  enum Animations frame;
} Actor;

typedef struct actorlist {
  struct Actor *actor;
  struct actorlist *next;
} actorlist;

Actor *createActor(Sprite **spritedata, const char* name, int type, int state);
void setActorFrame(Actor *a, int frame); 
void updateActorFrame(Actor *a);
void addActor(actorlist **Actors, Actor *a);
void setActorXY(Actor *a, int x, int y);
void moveActorXY(Actor *a, float x, float y);
