#include <SDL.h>

typedef struct {
  int w, h;
  uint8_t* pixels;
} framebuffer;

framebuffer *initFireEffect(int fire_width, int fire_height);
SDL_Texture *fireEffectTex(); 
