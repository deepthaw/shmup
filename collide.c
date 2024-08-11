#include "collide.h"
#include "game.h"
#include <SDL.h>
#include <stdbool.h>

extern gameData game;

SDL_Rect getHitBox(Actor *a) {
  SDL_Rect r;
  r.x = a->x + a->sprite->hitbox.x;
  r.y = a->y + a->sprite->hitbox.y;
  r.w = a->sprite->hitbox.w;
  r.h = a->sprite->hitbox.h;
  return r;
}

bool checkHitTiles(Actor *a, int xoffset, int yoffset) {
  SDL_Rect r = getHitBox(a);

  int x = (r.x + game.x + xoffset) / TILE_X;
  int y = (r.y + game.y + yoffset) / TILE_Y;
  int xw = (r.x + r.w + game.x + xoffset) / TILE_X;
  int yh = (r.y + r.h + game.y + yoffset) / TILE_Y;
  tmx_layer *layer = tmx_find_layer_by_name(map, "bg1");
  return (layer->content.gids[y * map->width + x] ||
          layer->content.gids[y * map->width + xw] ||
          layer->content.gids[yh * map->width + x] ||
          layer->content.gids[yh * map->width + xw]);
}

bool checkCollision(Actor *a, Actor *b) {
  SDL_Rect r1 = getHitBox(a);
  SDL_Rect r2 = getHitBox(b);

  return (r1.x < r2.x + r2.w && r1.x + r1.w > r2.x && r1.y < r2.y + r2.h &&
          r1.y + r1.h > r2.y);
}
