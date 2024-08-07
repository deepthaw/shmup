#include <SDL.h>

typedef struct {
  int w, h;
  uint8_t* pixels;
} framebuffer;

framebuffer *initFireEffect(int fire_width, int fire_height);
void spreadFire(int from, framebuffer *fire);
SDL_Texture *texFromFire(framebuffer* fire); 
