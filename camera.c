#include "camera.h"
#include "graphics.h"

extern renderList *buffers; 

bool checkScroll(tmx_map *map) {

  return false;
}

void scrollScreen(tmx_map *map, int x, int y, int x_offsetpx, int y_offsetpx,
                  int x_size, int y_size) {

  struct SDL_Rect r;
  r.h = TILE_Y;
  r.w = TILE_X;

  renderList *buffer = buffers;
  while (buffer) {
    if (strcmp(buffer->layer->name, "enemylayer") != 0) {
      buffer->screen[0] = buffer->screen[1];
      buffer->screen[1] = draw_layer(map, buffer->layer, x / TILE_X, y / TILE_Y,
                                     x_size, y_size);
      r.x = r.y = 0;
      r.h = y_size * TILE_Y;
      r.w = x_size * TILE_X;
      SDL_SetRenderTarget(gRenderer, buffer->buffer);
      SDL_RenderCopy(gRenderer, buffer->screen[0], NULL, &r);
      r.x = x_offsetpx;
      r.y = y_offsetpx;
      SDL_RenderCopy(gRenderer, buffer->screen[1], NULL, &r);
    }
    buffer = buffer->next;
  }
  SDL_SetRenderTarget(gRenderer, NULL);
}
