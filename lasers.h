#include "actors.h"
#include "game.h"

extern Actor* player;
extern actorlist *lasers;

Frame *drawLaser(Actor *a);
Actor *createLaser(int x, int y);
void fireLaser();

