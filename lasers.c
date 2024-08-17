#include "lasers.h"
#include "game.h"

#define laser_width 240
#define laser_speed 36
#define laser_body 96
#define laser_head 8

extern gameData game;
Frame *drawLaser(Actor *a) {
  uint32_t *laserpixels = calloc(3 * laser_width, sizeof(uint32_t));
  int w = a->elapsed;
  for (int i = 0; i < w + laser_speed && i < laser_width; i++) {
    if (i >= w + laser_speed - laser_head) {
      laserpixels[i + laser_width] = 0xFFFFFFFF;
      laserpixels[i] = 0x22FFFFFF;
      laserpixels[i + (laser_width * 2)] = 0x22FFFFFF;
    } else if (i >= w + laser_speed - laser_body) {
      laserpixels[i + laser_width] = 0xFF00FFFF;
    } else if (!(i % 5)) {
      laserpixels[i + laser_width] = 0xFF00FFFF;
    }
  }

  SDL_Surface *laserSurface = SDL_CreateRGBSurfaceWithFormatFrom(
      laserpixels, laser_width, 3, 32, laser_width * 4, SDL_PIXELFORMAT_BGRA32);
  SDL_Texture *laserTex = SDL_CreateTextureFromSurface(gRenderer, laserSurface);

  SDL_Rect r = {0, 0, laser_width, 3};
  Frame *laserFrame = malloc(sizeof(Frame));
  laserFrame->tex = laserTex;
  laserFrame->rect = r;
  a->elapsed += 8;
  return laserFrame;
}

Actor *createLaser(int x, int y) {
  printf("spawned shot!\n");
  Actor *laser = calloc(1, sizeof(Actor));
  laser->currentframe = malloc(sizeof(Animation));
  laser->currentframe->frame = drawLaser(laser);
  laser->x = x;
  laser->y = y;
  return laser;
}

void fireLaser() {
  if (game.laserCooldown >= 30) {
    Actor *laser = createLaser(player->x + 16, player->y + 16);
    addActor(&lasers, laser);
    game.laserCooldown = 0;
  }
}
