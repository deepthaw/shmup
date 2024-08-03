#include "main.h"
#include "sprites.h"
#include "tmx.h"
#include "tmx_render.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdbool.h>
#include <stdio.h>

SDL_Window *gWindow = NULL;

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
void togglePause();
// tmx_resource_manager *resources;
spritelist *initializeSprites();
Sprite *createSprite(char *name);
void addSprite(spritelist *sprites, Sprite *s);
bool allocateMemory();

tmx_map *map;
gameData game;
renderList *buffers;
spritelist *sprites;
Sprite *player;

SDL_Renderer *gRenderer = NULL;

void scrollScreen(tmx_map *map, int x, int y, int x_offsetpx, int y_offsetpx,
                  int x_size, int y_size);
void initializeBackground();

int screen_height_tiles = SCREEN_HEIGHT / TILE_Y;
int screen_width_tiles = SCREEN_WIDTH / TILE_X;

int player_holdup, player_holddown, player_holdright, player_holdleft = 0;
bool SCROLL_STOP = false;
bool PAUSED = false;

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
void togglePause() { PAUSED = !PAUSED; }

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

  map = tmx_load("MAP1.TMX");
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
    player->rect.y--;
    player_holdup++;
  } else {
    player_holdup -= 6;
    if (player_holdup < 0)
      player_holdup = 0;
  }
  if (keys[SDL_SCANCODE_DOWN]) {
    player->rect.y++;
    player_holddown++;
  } else {
    player_holddown -= 6;
    if (player_holddown < 0)
      player_holddown = 0;
  };
  if (keys[SDL_SCANCODE_LEFT]) {
    player->rect.x--;
    player_holdleft++;
  }
  if (keys[SDL_SCANCODE_RIGHT]) {
    player->rect.x++;
    player_holdright++;
  }
  if (keys[SDL_SCANCODE_R]) {
    resetView();
  }
  if (keys[SDL_SCANCODE_P]) {
    togglePause();
  }
}

bool initializeView(SDL_Rect *view) {
  view->x = view->y = 0;
  view->h = SCREEN_HEIGHT;
  view->w = SCREEN_WIDTH;
  return true;
}

bool allocateMemory() {
  bool success = true;
  sprites = initializeSprites();
  return success;
}

void checkSpawns(int cell_x);

void spawnEnemyAt(int32_t GID, int cell_x, int cell_y);

void checkSpawns(int cell_x) {
  tmx_layer *layer = tmx_find_layer_by_name(map, "enemylayer");
  for (int cell_y = 0; cell_y != map->height; cell_y++) {
    int32_t cell = layer->content.gids[cell_y * map->width + cell_x];
    printf("x: %d y:%d cell:%d\n",cell_x,cell_y,cell);
    int32_t GID = cell & TMX_FLIP_BITS_REMOVAL;
    if (GID) {
      printf("ENEMY SPAWNED AT %d %d\n", cell_x, cell_y);
      spawnEnemyAt(GID, cell_x, cell_y);
    }
  }
}

void spawnEnemyAt(int32_t GID, int cell_x, int cell_y) {
  
  char *enemy_type = map->tiles[GID]->type;
  Sprite *enemy = createSprite(enemy_type);
  int x = cell_x * TILE_X + (cell_x * TILE_X % SCREEN_WIDTH);
  int y = cell_y * TILE_Y;
  enemy->rect.x = x;
  enemy->rect.y = y;
  addSprite(sprites, enemy);
}

void update() {

  if (game.x % TILE_X == 0) {
    checkSpawns(game.x / TILE_X);
  }

  if (!PAUSED) {
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

    if (view.x >= SCROLL_STEP) {
      printf("scrolling at view.x %d\n", view.x);
      scrollScreen(map, game.x, game.y, view.x, view.y, SCROLL_STEP / TILE_X,
                   SCREEN_HEIGHT / TILE_Y);
      view.x = view.x - SCROLL_STEP;
    }
  }
  return;
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
    buffer->screen[1] =
        draw_layer(map, buffer->layer, x / TILE_X, y / TILE_Y, x_size, y_size);
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

void resetView() { view.x = 0; }

void initializeBackGround() {

  struct SDL_Rect r;
  r.h = TILE_Y;
  r.w = TILE_X;

  printf("beginning initializeBackGround\n");
  renderList *buffer = buffers;
  while (buffer) {
    if (strcmp(buffer->layer->name, "enemylayer") != 0) {
      printf("layer: %s\n", buffer->layer->name);
      buffer->screen[0] =
          draw_layer(map, buffer->layer, 0, 0, SCREEN_WIDTH / TILE_X,
                     SCREEN_HEIGHT / TILE_Y);
      buffer->screen[1] =
          draw_layer(map, buffer->layer, screen_width_tiles, 0,
                     SCREEN_WIDTH / TILE_X, SCREEN_HEIGHT / TILE_Y);
      r.x = r.y = 0;
      r.h = SCREEN_HEIGHT;
      r.w = SCREEN_WIDTH;
      SDL_SetRenderTarget(gRenderer, buffer->buffer);
      SDL_RenderCopy(gRenderer, buffer->screen[0], NULL, &r);
      r.x = SCREEN_WIDTH;
      SDL_RenderCopy(gRenderer, buffer->screen[1], NULL, &r);
    }
    printf("next: %d\n", buffer->next);
    buffer = buffer->next;
  }
  SDL_SetRenderTarget(gRenderer, NULL);
  printf("ending initializebackground\n");
}

spritelist *initializeSprites() {
  sprites = malloc(sizeof(spritelist));
  sprites->sprite = malloc(sizeof(Sprite));
  sprites->next = NULL;
  return sprites;
}

Sprite *createSprite(char *name) {
  Sprite *s = malloc(sizeof(Sprite));
  s->frames = calloc(NUMSTATES, sizeof(SDL_Texture *));
  tmx_tileset *tileset = tmx_find_tileset_by_name(map, name)->tileset;
  int tilecount = tileset->tilecount;
  tmx_tile *tiles = tileset->tiles;

  for (int i = 0; i != NUMSTATES; i++) {
    s->frames[i] = calloc(1, sizeof(spriteTex));
  }

  for (int i = 0; i != tilecount; i++) {
    tmx_property prop = *tmx_get_property(tiles[i].properties, "state");
    tmx_image *image = tiles[i].image;
    char *source = image->source;
    for (int j = 0; j != NUMSTATES; j++) {
      printf("state: %s", prop.value.string);
      if (strcmp(prop.value.string, stateTable[j]) == 0) {
        printf("source: %s\n", source);
        s->frames[j]->tex = IMG_LoadTexture(gRenderer, source);
        s->frames[j]->rect.h = tiles[i].height;
        s->frames[j]->rect.w = tiles[i].width;
      } else {
        printf(" not found.\n");
      }
    }
  }
  setSpriteState(s, SPNEUTRAL);

  return s;
}

void addSprite(spritelist *sprites, Sprite *s) {
  spritelist *node = sprites;
  while (node->next) {
    node = node->next;
  }
  node->next = malloc(sizeof(spritelist));
  node = node->next;
  node->sprite = s;
  node->next = NULL;
}

void renderSprites() {
  printf("rendering sprites\n");
  spritelist *list = sprites;
  while (list) {
    SDL_RenderCopy(gRenderer, list->sprite->tex, NULL, &list->sprite->rect);
    list = list->next;
  }
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
  printf("initializing view\n");
  initializeView(&view);
  printf("allocating memory.\n");
  allocateMemory();
  printf("creating player.\n");
  player = createSprite("P_VIPER");
  printf("adding player.\n");
  addSprite(sprites, player);
  player->rect.x = 0;
  player->rect.y = 120;
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
