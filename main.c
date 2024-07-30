#include "main.h"
#include "glib.h"
#include "map.h"
#include "sprites.h"
#include "tiles.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdbool.h>
#include <stdio.h>

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

SDL_Texture *bgScreens[2];
SDL_Texture *bgLayer[BG_LAYERS];

SDL_Rect view;

bool initializeGraphics();
bool loadMedia();
void processInput();
void update();
void render();
void renderSprites();
void renderBackgrounds();

void resetView();

Sprite *player;
Map *map;

GHashTable *tileSet;
GHashTable *spriteSet;

SDL_Texture *buildBackGround();
SDL_Texture *scrollExceed(SDL_Texture *append);

int player_holdup, player_holddown, player_holdright, player_holdleft = 0;

bool initializeGraphics() {
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    success = false;
  } else {
    printf("creating window.\n");
    gWindow = SDL_CreateWindow("shmup", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCALE,
                               SCREEN_HEIGHT * SCALE, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
      success = false;
    } else {
      printf("creating renderer\n");
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        success = false;
      } else {
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not initialize! SDL_image Error: %s\n",
                 SDL_GetError());
          success = false;
        } else {

          bgScreens[0] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB24,
                                           SDL_TEXTUREACCESS_TARGET,
                                           SCREEN_WIDTH, SCREEN_HEIGHT);

          bgScreens[1] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB24,
                                           SDL_TEXTUREACCESS_TARGET,
                                           SCREEN_WIDTH, SCREEN_HEIGHT);

          bgLayer[0] = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB24,
                                         SDL_TEXTUREACCESS_TARGET,
                                         SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2);
        }
      }
    }

    printf("init success\n");
  }

  return success;
}

bool loadMedia() {

  bool success = true;

  spriteSet = loadSprites("sprites.tsx");
  tileSet = loadTiles("tiles.tsx");
  bgLayer[0] = buildBackGround();
  map = loadMap("map1.tmx");
  for (int i = 0; i != map->rows; i++) {
    for (int j = 0; j != map->columns; j++) {
      printf("%s",map->tile[i][j]);
    }
    printf("\n");
  }



  printf("loadMedia success\n");
  return success;
}

/*
void close() {

  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  SDL_Quit();
}
*/

void processInput() {
  const Uint8 *keys = SDL_GetKeyboardState(NULL);

  if (keys[SDL_SCANCODE_UP]) {
    player->y--;
    player_holdup++;
  } else {
    player_holdup -= 6;
    if (player_holdup < 0)
      player_holdup = 0;
  }
  if (keys[SDL_SCANCODE_DOWN]) {
    player->y++;
    player_holddown++;
  } else {
    player_holddown -= 6;
    if (player_holddown < 0)
      player_holddown = 0;
  };
  if (keys[SDL_SCANCODE_LEFT]) {
    player->x--;
    player_holdleft++;
  }
  if (keys[SDL_SCANCODE_RIGHT]) {
    player->x++;
    player_holdright++;
  }
  if (keys[SDL_SCANCODE_R]) {
    resetView();
  }
}

void update() {
  view.x += SCROLL_SPEED;
  view.y = 0;
  view.w = SCREEN_WIDTH;
  view.h = SCREEN_HEIGHT;
  player->rect.x = player->x;
  player->rect.y = player->y;

  if (player_holdup > 30) {
    //      player->tex = spriteTexture[2];
  } else if (player_holdup > 15) {
    //      player->tex = spriteTexture[1];
  } else if (player_holddown > 30) {
    //      player->tex = spriteTexture[4];
  } else if (player_holddown > 15) {
    //      player->tex = spriteTexture[3];
  } else {
    //      player->tex = spriteTexture[0];
  }

  if (view.x > SCREEN_WIDTH) {
    bgLayer[0] = scrollExceed(NULL);
    view.x = view.x - SCREEN_WIDTH;
  }
  return;
}

void resetView() { view.x = 0; }

SDL_Texture *buildBackGround() {

  struct SDL_Rect r;
  r.h = TILE_Y;
  r.w = TILE_X;

  SDL_SetRenderTarget(gRenderer, bgScreens[0]);
  for (int i = 0; i != BG_LAYERS; i++) {
    for (int y = 0; y <= SCREEN_HEIGHT; y += TILE_Y) {
      for (int x = 0; x <= SCREEN_WIDTH; x += TILE_X) {
        r.x = x;
        r.y = y;
        Tile *t = g_hash_table_lookup(tileSet, "0");
        SDL_RenderCopy(gRenderer, t->tex, NULL, &r);
      }
    }
  }

  SDL_SetRenderTarget(gRenderer, bgScreens[1]);
  for (int i = 0; i != BG_LAYERS; i++) {
    for (int y = 0; y <= SCREEN_HEIGHT; y += TILE_Y) {
      for (int x = 0; x <= SCREEN_WIDTH; x += TILE_X) {
        r.x = x;
        r.y = y;
        Tile *t = g_hash_table_lookup(tileSet, "1");
        SDL_RenderCopy(gRenderer, t->tex, NULL, &r);
      }
    }
  }

  r.x = r.y = 0;
  r.h = SCREEN_HEIGHT;
  r.w = SCREEN_WIDTH;
  SDL_SetRenderTarget(gRenderer, bgLayer[0]);
  SDL_RenderCopy(gRenderer, bgScreens[0], NULL, &r);
  r.x = SCREEN_WIDTH;
  SDL_RenderCopy(gRenderer, bgScreens[1], NULL, &r);
  SDL_SetRenderTarget(gRenderer, NULL);
  return bgLayer[0];
}

SDL_Texture *scrollExceed(SDL_Texture *append) {

  struct SDL_Rect r;
  r.h = TILE_Y;
  r.w = TILE_X;

  SDL_Texture *p = bgScreens[0];
  bgScreens[0] = bgScreens[1];
  bgScreens[1] = p;

  /*  SDL_SetRenderTarget(gRenderer, bgScreens[1]);
    for (int i = 0; i != BG_LAYERS; i++) {
      for (int y = 0; y <= SCREEN_HEIGHT; y += TILE_Y) {
        for (int x = 0; x <= SCREEN_WIDTH; x += TILE_X) {
          r.x = x;
          r.y = y;
          SDL_RenderCopy(gRenderer, bgTiles[2], NULL, &r);
        }
      }
    }
  */
  r.x = r.y = 0;
  r.h = SCREEN_HEIGHT;
  r.w = SCREEN_WIDTH;
  SDL_SetRenderTarget(gRenderer, bgLayer[0]);
  SDL_RenderCopy(gRenderer, bgScreens[0], NULL, &r);
  r.x = SCREEN_WIDTH;
  SDL_RenderCopy(gRenderer, bgScreens[1], NULL, &r);
  SDL_SetRenderTarget(gRenderer, NULL);
  return bgLayer[0];
}

void renderBackgrounds() {}

void renderSprites() {
  SDL_RenderCopy(gRenderer, player->tex, NULL, &(player->rect));
}

void render() {

  SDL_RenderClear(gRenderer);
  SDL_RenderCopy(gRenderer, bgLayer[0], &view, NULL);

  renderBackgrounds();
  renderSprites();
  SDL_RenderSetScale(gRenderer, SCALE, SCALE);
  SDL_RenderPresent(gRenderer);
}

int main(int argc, char *args[]) {

  SDL_Event e;
  bool quit = false;
  uint64_t nextTic;
  if (!initializeGraphics()) {
    printf("Failed to initialize!\n");
    return 1;
  }
  if (!loadMedia()) {
    printf("Failed to load media!\n");
    return 1;
  }

  player = g_hash_table_lookup(spriteSet, "0");
  player->x = SCREEN_WIDTH / 2;
  player->y = SCREEN_HEIGHT / 2;
  player->rect.x = player->x;
  player->rect.y = player->y;

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }
    nextTic = SDL_GetTicks64() + SCREEN_TICKS_PER_FRAME;
    processInput();
    update();
    render();
    while (SDL_GetTicks64() <= nextTic)
      ;
  }

  return 0;
}
