#include <SDL.h>
#include <stdbool.h>
#include "actors.h"

SDL_Rect getHitBox(Actor *a);
bool checkHitTiles(Actor *a, int xoffset, int yoffset);
bool checkCollision(Actor *a, Actor *b);
