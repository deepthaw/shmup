#include <SDL.h>
#include "fire.h"

extern SDL_Renderer *gRenderer;

uint32_t firePalette[] = {
    0x00070707, 0xFF1F0707, 0xFF2F0F07, 0xFF470F07, 0xFF571707, 0xFF671F07,
    0xFF771F07, 0xFF8F2707, 0xFF9F2F07, 0xFFAF3F07, 0xFFBF4707, 0xFFC74707,
    0xFFDF4F07, 0xFFDF5707, 0xFFDF5707, 0xFFD75F07, 0xFFD75F07, 0xFFD7670F,
    0xFFCF6F0F, 0xFFCF770F, 0xFFCF7F0F, 0xFFCF8717, 0xFFC78717, 0xFFC78F17,
    0xFFC7971F, 0xFFBF9F1F, 0xFFBF9F1F, 0xFFBFA727, 0xFFBFA727, 0xFFBFAF2F,
    0xFFB7AF2F, 0xFFB7B72F, 0xFFB7B737, 0xFFCFCF6F, 0xFFDFDF9F, 0xFFEFEFC7,
    0xFFFFFFFF};

static framebuffer *fire;
static int firetimer;

framebuffer *initFireEffect(int fire_width, int fire_height) {
  fire = malloc(sizeof(framebuffer));
  fire->w = fire_width;
  fire->h = fire_height;
  fire->pixels = calloc(fire->w * fire->h, sizeof(*fire->pixels));

  for (int y = 0; y != fire->h; y++) {
    for (int x = 0; x != fire->w; x++) {
      if (y == fire->h - 1) {
        fire->pixels[fire->w * y + x] = 36;
      } else {
        fire->pixels[fire->w * y + x] = 0;
      }
    }
  }
  return fire;
}

void spreadFire(int from) {
  int r = rand() % 4;
  int to = from - fire->w - r + 1;
  fire->pixels[to] = fire->pixels[from] - r;
  if (fire->pixels[to] > 36)
    fire->pixels[to] = 0;
}

SDL_Texture *fireEffectTex() {

  if (++firetimer > 0)
  {
    for (int x = 0; x < fire->w; x++) {
      for (int y = 1; y < fire->h; y++) {
        spreadFire(y * fire->w + x);
      }
    }
    firetimer = 0;
  }

    uint32_t *fireargb = calloc(fire->w * fire->h, sizeof(uint32_t));
    SDL_Surface *fireSurface;
    for (int y = 0; y != fire->h; y++) {
      for (int x = 0; x != fire->w; x++) {
        fireargb[fire->w * y + x] =
            firePalette[fire->pixels[fire->w * y + x]];
      }
    }
  
  fireSurface = SDL_CreateRGBSurfaceWithFormatFrom(
      fireargb, fire->w, fire->h, 32, fire->w * 4,
      SDL_PIXELFORMAT_BGRA32);
  SDL_Texture *fireTex = SDL_CreateTextureFromSurface(gRenderer, fireSurface);
  return fireTex;
}
