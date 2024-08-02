#include "main.h"
#include "glib.h"
#include "sprites.h"
#include "tmx.h"
#include "tmx_render.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

SDL_Window *gWindow = NULL;

SDL_Texture *bgScreens[BG_LAYERS][2];
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
bool initializeView(SDL_Rect *view);
bool initializeGame(gameData *game);
bool initializeBuffers();
renderList *initializeRenderList();

Sprite player;
tmx_map *map;
gameData game;
renderList *buffers;

SDL_Renderer *gRenderer = NULL;

void scrollScreen(tmx_map *map, int x, int y, int x_offsetpx, int y_offsetpx,
                  int x_size, int y_size);
void initializeBackground();

int screen_height_tiles = SCREEN_HEIGHT / TILE_Y;
int screen_width_tiles = SCREEN_WIDTH / TILE_X;

int player_holdup, player_holddown, player_holdright, player_holdleft = 0;
bool SCROLL_STOP = false;

renderList *initializeRenderList() {
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
      gRenderer = SDL_CreateRenderer(
          gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n",
               SDL_GetError());
        success = false;
      } else {
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(gRenderer, 128, 128, 128, 255);
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not initialize! SDL_image Error: %s\n",
                 SDL_GetError());
          success = false;
        }
      }
    }
  }
  return success;
}

bool initializeGame(gameData *game) {
  game->x = game->y = 0;
  return true;
}

bool loadMedia() {

  bool success = true;

  map = tmx_load("map1.tmx");
  if (map == NULL) {
    tmx_perror("Cannot load map");
    return 1;
  }
  printf("loadMedia success\n");
  return success;
}

void processInput() {
  const Uint8 *keys = SDL_GetKeyboardState(NULL);

  if (keys[SDL_SCANCODE_UP]) {
    player.y--;
    player_holdup++;
  } else {
    player_holdup -= 6;
    if (player_holdup < 0)
      player_holdup = 0;
  }
  if (keys[SDL_SCANCODE_DOWN]) {
    player.y++;
    player_holddown++;
  } else {
    player_holddown -= 6;
    if (player_holddown < 0)
      player_holddown = 0;
  };
  if (keys[SDL_SCANCODE_LEFT]) {
    player.x--;
    player_holdleft++;
  }
  if (keys[SDL_SCANCODE_RIGHT]) {
    player.x++;
    player_holdright++;
  }
  if (keys[SDL_SCANCODE_R]) {
    resetView();
  }
  printf("playerx: %d\n", player.x);
}

bool initializeView(SDL_Rect *view) {
  view->x = view->y = 0;
  view->h = SCREEN_HEIGHT;
  view->w = SCREEN_WIDTH;
  return true;
}

void update() {

  if (game.x / TILE_X + SCREEN_WIDTH / TILE_X >= map->width) {
    SCROLL_STOP = true;
  } else {
    SCROLL_STOP = false;
  }

  if (!SCROLL_STOP) {
    game.x += SCROLL_SPEED;
    view.x += SCROLL_SPEED;
    game.y = 0;
    view.y = 0;
  }
  player.rect.x = player.x;
  player.rect.y = player.y;

  if (view.x >= SCROLL_STEP) {
    printf("scrolling at view.x %d\n", view.x);
    scrollScreen(map, game.x, game.y, view.x, view.y, SCROLL_STEP / TILE_X,
                 SCREEN_HEIGHT / TILE_Y);
    view.x = view.x - SCROLL_STEP;
  }
  return;
}

void resetView() { view.x = 0; }

void initializeBackGround() {

  struct SDL_Rect r;
  r.h = TILE_Y;
  r.w = TILE_X;

  printf("beginning initializeBackGround\n");
  renderList *buffer = buffers;
  while (buffer) {
    buffer->screen[0] = draw_layer(map, buffer->layer, 0, 0, SCREEN_WIDTH / TILE_X,
                                 SCREEN_HEIGHT / TILE_Y);
    buffer->screen[1] = draw_layer(map, buffer->layer, screen_width_tiles, 0,
                                 SCREEN_WIDTH / TILE_X, SCREEN_HEIGHT / TILE_Y);
    r.x = r.y = 0;
    r.h = SCREEN_HEIGHT;
    r.w = SCREEN_WIDTH;
    SDL_SetRenderTarget(gRenderer, buffer->buffer);
    SDL_RenderCopy(gRenderer, buffer->screen[0], NULL, &r);
    r.x = SCREEN_WIDTH;
    SDL_RenderCopy(gRenderer, buffer->screen[1], NULL, &r);
    printf("next: %d\n",buffer->next);
    buffer = buffer->next;
  }
  SDL_SetRenderTarget(gRenderer, NULL);
  printf("ending initializebackground\n");
}

void scrollScreen(tmx_map *map, int x, int y, int x_offsetpx, int y_offsetpx,
                  int x_size, int y_size) {

  struct SDL_Rect r;
  r.h = TILE_Y;
  r.w = TILE_X;

  printf("swapping\n");
  renderList *buffer = buffers;
  while (buffer) {
    buffer->screen[0] = buffer->screen[1];
    buffer->screen[1] = draw_layer(map, buffer->layer, x / TILE_X, y / TILE_Y, x_size, y_size);
    printf("swapped\n");
    r.x = r.y = 0;
    r.h = y_size * TILE_Y;
    r.w = x_size * TILE_X;
    SDL_SetRenderTarget(gRenderer, buffer->buffer);
    SDL_RenderCopy(gRenderer, buffer->screen[0], NULL, &r);
    r.x = x_offsetpx;
    r.y = y_offsetpx;
    SDL_RenderCopy(gRenderer, buffer->screen[1], NULL, &r);
    buffer = buffer->next;
  }
  SDL_SetRenderTarget(gRenderer, NULL);
}

void renderSprites() {
  SDL_RenderCopy(gRenderer, player.tex, NULL, &player.rect);
}

void render() {

  SDL_RenderClear(gRenderer);
 renderList *buffer = buffers;
  while (buffer) {
    SDL_Rect r = view;
    r.x = view.x * buffer->layer->parallaxx;
    r.y = view.y * buffer->layer->parallaxy;
    SDL_RenderCopy(gRenderer, buffer->buffer, &r, NULL);
    buffer = buffer->next;
  }

  renderSprites();
  SDL_RenderSetScale(gRenderer, SCALE, SCALE);
  SDL_RenderPresent(gRenderer);
}

int main(int argc, char *args[]) {

  // libtmx callbacks for drawing tiles
  tmx_img_load_func = SDL_tex_loader;
  tmx_img_free_func = (void (*)(void *))SDL_DestroyTexture;

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

  buffers = initializeRenderList();
  if (buffers == NULL) {
    printf("Failed to initialize graphic buffers!\n");
    return 1;
  }

  if (!initializeGame(&game)) {
    printf("Failed to initialize game state!\n");
    return 1;
  }
  initializeBackGround();
  initializeView(&view);

  printf("loading player\n");
  player.tex = IMG_LoadTexture(gRenderer, "images/sprite000.png");
  printf("loaded texture\n");
  player.x = 0;
  printf("player x %d\n", player.x);
  player.y = 120;
  player.rect.x = player.x;
  player.rect.y = player.y;
  player.rect.w = 30;
  player.rect.h = 11;
  printf("done loading\n");
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
