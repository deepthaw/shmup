#include "camera.h"
#include "collide.h"
#include "fire.h"
#include "game.h"
#include "graphics.h"
#include "lasers.h"
#include "sound.h"
#include "tmx.h"
#include "tmx_render.h"
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

SDL_Window *gWindow = NULL;
SDL_FRect view;

bool loadMedia();
void processInput();
void update();
void render();
void renderSprites();
void renderBackgrounds();
void resetView();
bool initializeView(SDL_FRect *view);
bool initializeGame(gameData *game);
bool initializeBuffers();
void togglePause();
void checkSpawns(int cell_x);
void spawnEnemyAt(int32_t GID, int cell_x, int cell_y);

tmx_map *map;
renderList *buffers;
actorlist *actors = NULL;
actorlist *lasers = NULL;
Actor *player = NULL;
SDL_Renderer *gRenderer = NULL;
Sprite **spritedata;
gameData game;
void initializeBackground();

int screen_height_tiles = SCREEN_HEIGHT / TILE_Y;
int screen_width_tiles = SCREEN_WIDTH / TILE_X;
int player_holdup, player_holddown, player_holdright, player_holdleft = 0;
bool SCROLL_STOP = false;
bool PAUSED = false;

void togglePause() { PAUSED = !PAUSED; }

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
  int x, y;
  x = y = 0;
  if (keys[SDL_SCANCODE_UP]) {
    y--;
    player_holdup++;
    if (player_holdup > 60) {
      player_holdup = 60;
    }
  } else {
    player_holdup--;
    if (player_holdup < 0)
      player_holdup = 0;
  }
  if (keys[SDL_SCANCODE_DOWN]) {
    y++;
    player_holddown++;
    if (player_holddown > 60) {
      player_holddown = 60;
    }
  } else {
    player_holddown--;
    if (player_holddown < 0)
      player_holddown = 0;
  };
  if (keys[SDL_SCANCODE_LEFT]) {
    x--;
    player_holdleft++;
  }
  if (keys[SDL_SCANCODE_RIGHT]) {
    x++;
    player_holdright++;
  }
  if (keys[SDL_SCANCODE_LCTRL]) {
    fireLaser();
  }
  if (checkHitTiles(player, 0, y)) {
    y = 0;
  }
  if (checkHitTiles(player, x, 0)) {
    x = 0;
  }

  if (x || y) {
    moveActorXY(player, x, y);
  }
}

bool initializeView(SDL_FRect *view) {
  view->x = view->y = 0;
  view->h = SCREEN_HEIGHT;
  view->w = SCREEN_WIDTH;
  return true;
}

void checkSpawns(int cell_x) {
  tmx_layer *layer = tmx_find_layer_by_name(map, "enemylayer");
  for (int cell_y = 0; cell_y != map->height; cell_y++) {
    int32_t cell = layer->content.gids[cell_y * map->width + cell_x];
    int32_t GID = cell & TMX_FLIP_BITS_REMOVAL;
    if (GID) {
      printf("spawning enemy at %u %u\n", cell_x, cell_y);
      spawnEnemyAt(GID, cell_x, cell_y);
    }
  }
}

void spawnEnemyAt(int32_t GID, int cell_x, int cell_y) {
  Actor *enemy = createActor(spritedata, "E_ROLLER", ENEMY, 0);
  int x = cell_x * TILE_X + (cell_x * TILE_X % SCREEN_WIDTH);
  int y = cell_y * TILE_Y;
  setActorXY(enemy, x, y);
  addActor(&actors, enemy);
}

void update() {

  if (++game.laserCooldown > 30) game.laserCooldown = 30;
  if ((int)game.x % TILE_X == 0) {
    checkSpawns(game.x / TILE_X);
  }

  if ((game.x + SCREEN_WIDTH) / TILE_X >= map->width) {
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
    scrollScreen(map, game.x + SCROLL_STEP, game.y, view.x, view.y,
                 SCROLL_STEP / TILE_X, SCREEN_HEIGHT / TILE_Y);
    view.x = view.x - SCROLL_STEP;
  }

  actorlist *current = actors;
  while (current) {
    if (current->actor->type != PLAYER) {
      if (checkCollision(player, current->actor)) {
      }
    }
    current = current->next;
  }

  if (checkHitTiles(player, 0, 0)) {
    moveActorXY(player, -SCROLL_SPEED, 0);
  }
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
    buffer = buffer->next;
  }
  SDL_SetRenderTarget(gRenderer, NULL);
  printf("ending initializebackground\n");
}

void renderShots() {
  actorlist *current = lasers;
  while (current != NULL) {
    Actor *a = current->actor;
    Frame *laserFrame = drawLaser(a);
    SDL_SetTextureBlendMode(laserFrame->tex, SDL_BLENDMODE_ADD);
    a->x = a->x + 8;
    SDL_Rect r = {a->x, a->y, laserFrame->rect.w, laserFrame->rect.h};
    SDL_RenderCopy(gRenderer, laserFrame->tex, NULL, &r);
    if (a->elapsed >= 1960) {
      lasers = current->next;
      free(current);
      current = lasers;
      printf("laser removed!\n");
    } else {
      current = current->next;
    }
  }
}

void renderActors() {
  actorlist *current = actors;
  while (current != NULL) {
    Actor *a = current->actor;
    if (a->type == ENEMY) {
      a->x = a->x - 2;
    }
    if (a->type == PLAYER) {
      if (player_holdup - player_holddown > 40) {
        setActorFrame(a, SPFULLUP);
      } else if (player_holdup - player_holddown > 20) {
        setActorFrame(a, SPTILTUP);
      } else if (player_holddown - player_holdup > 40) {
        setActorFrame(a, SPFULLDN);
      } else if (player_holddown - player_holdup> 20) {
        setActorFrame(a, SPTILTDN);
      } else if (a->frame != SPNEUTRAL) {
        setActorFrame(a, SPNEUTRAL);
      }
    }
    SDL_Rect r = {a->x, a->y, a->sprite->rect.w, a->sprite->rect.h};
    SDL_RenderCopy(gRenderer, a->currentframe->frame->tex, NULL, &r);
    updateActorFrame(a);
    current = current->next;
  }
}

int init() {
  srand(time(NULL));

  if (!initSound()) {
    printf("Failed to initialize sound!\n");
    return 1;
  }

  gRenderer = initializeGraphics();
  if (!gRenderer) {
    printf("Failed to initialize!\n");
    return 1;
  }

  if (!loadMedia()) {
    printf("Failed to load media!\n");
    return 1;
  }

  spritedata = loadSpritesFromDisk(spriteNames, NUMSPRITENAMES);
  if (spritedata == NULL) {
    printf("Failed to load sprite data!\n");
    return 1;
  }

  buffers = initializeRenderList(map);
  if (buffers == NULL) {
    printf("Failed to initialize graphic buffers!\n");
    return 1;
  }

  if (!initializeGame(&game)) {
    printf("Failed to initialize game state!\n");
    return 1;
  }

  initFireEffect(320, 240);
  initializeBackGround();
  initializeView(&view);
  player = createActor(spritedata, "P_VIPER", PLAYER, 0);
  setActorXY(player, 0, 120);
  addActor(&actors, player);
  printf("Game initialization successful.");

  return 0;
}

void render() {

  SDL_RenderClear(gRenderer);
  renderList *buffer = buffers;
  while (buffer) {
    SDL_Rect r;
    r.h = (int)view.h;
    r.w = (int)view.w;
    r.x = (int)view.x * buffer->layer->parallaxx;
    r.y = (int)view.y * buffer->layer->parallaxy;
    SDL_RenderCopy(gRenderer, buffer->buffer, &r, NULL);
    buffer = buffer->next;
  }

  renderActors();
  // move fire elsewhere
  renderShots();
  SDL_RenderCopy(gRenderer, fireEffectTex(), NULL, NULL);
  SDL_RenderSetScale(gRenderer, SCALE, SCALE);
  SDL_RenderPresent(gRenderer);
}

int main(int argc, char *args[]) {

  // libmikmod music playback thread
  pthread_t tid;
  // libtmx callbacks for drawing tiles
  tmx_img_load_func = SDL_tex_loader;
  tmx_img_free_func = (void (*)(void *))SDL_DestroyTexture;

  SDL_Event e;
  bool quit = false;
  uint64_t nextTic;

  int initState = init();
  if (initState != 0) {
    return initState;
  }

  char *mod = "MUSIC/ELYSIUM.MOD";
  pthread_create(&tid, NULL, playMusic, (void *)mod);
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
      if (e.type == SDL_KEYDOWN) {
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_P]) {
          togglePause();
        }
      }
    }
    nextTic = SDL_GetTicks64() + SCREEN_TICKS_PER_FRAME;
    processInput();
    if (!PAUSED) {
      update();
      render();
    }
    while (SDL_GetTicks64() <= nextTic)
      ;
  }

  return 0;
}
