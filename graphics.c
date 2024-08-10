#include "graphics.h"

SDL_Renderer *initializeGraphics() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  } else {
    printf("creating window.\n");
    gWindow = SDL_CreateWindow("shmup", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCALE,
                               SCREEN_HEIGHT * SCALE, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
      printf("creating renderer\n");
      gRenderer = SDL_CreateRenderer(
          gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
      } else {
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not initialize! SDL_image Error: %s\n",
                 SDL_GetError());
        }
      }
    }
  }
  return gRenderer;
}

renderList *initializeRenderList(tmx_map *map) {
  renderList *buff = malloc(sizeof(renderList));
  renderList *head = buff;
  tmx_layer *layer = map->ly_head;
  while (layer) {
    buff->layer = layer;
    buff->screen[0] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32,
                                        SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH,
                                        SCREEN_HEIGHT);

    buff->screen[1] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32,
                                        SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH,
                                        SCREEN_HEIGHT);

    buff->buffer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA32,
                                     SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH * 2,
                                     SCREEN_HEIGHT * 2);

    SDL_SetTextureBlendMode(buff->screen[0], SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(buff->screen[1], SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(buff->buffer, SDL_BLENDMODE_BLEND);
    if (layer->next) {
      buff->next = malloc(sizeof(renderList));
      buff = buff->next;
    } else {
      buff->next = NULL;
      buff = buff->next;
    }
    layer = layer->next;
  }
  return head;
}

