#include "game.h"
#include "actors.h"
#include "fire.h"
#include "camera.h"
#include "graphics.h"
#include "sound.h"
#include "tmx.h"
#include "tmx_render.h"
#include <SDL.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

SDL_Window *gWindow = NULL;
SDL_Rect view;

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
void togglePause();
void checkSpawns(int cell_x);
void spawnEnemyAt(int32_t GID, int cell_x, int cell_y);

tmx_map *map;
gameData game;
renderList *buffers;
actorlist *actors = NULL;
Actor *player;
SDL_Renderer *gRenderer = NULL;
framebuffer *fire;
Sprite **spritedata;

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

void checkSpawns(int cell_x) {
  tmx_layer *layer = tmx_find_layer_by_name(map, "enemylayer");
  for (int cell_y = 0; cell_y != map->height; cell_y++) {
    int32_t cell = layer->content.gids[cell_y * map->width + cell_x];
    int32_t GID = cell & TMX_FLIP_BITS_REMOVAL;
    if (GID) {
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

SDL_Rect getHitBox(Actor *a) {
  SDL_Rect r;
  r.x = a->x + a->sprite->hitbox.x;
  r.y = a->y + a->sprite->hitbox.y;
  r.w = a->sprite->hitbox.w;
  r.h = a->sprite->hitbox.h;
  return r;
}

bool checkCollision(Actor *a, Actor *b) {
  SDL_Rect r1 = getHitBox(a);
  SDL_Rect r2 = getHitBox(b);

  return (r1.x < r2.x + r2.w && r1.x + r1.w > r2.x && r1.y < r2.y + r2.h &&
          r1.y + r1.h > r2.y);
}

bool checkHitTiles(Actor *a) {
  int x = (getHitBox(a).x + game.x) / TILE_X;
  int y = (getHitBox(a).y + game.y) / TILE_Y;
  printf("Player is in tile %u, %u\n", x, y);
  tmx_layer *layer = tmx_find_layer_by_name(map, "bg1");
  return layer->content.gids[y * map->width + x]; 
}

void update() {

  if (!PAUSED) {
    if (game.x % TILE_X == 0) {
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
      scrollScreen(map, game.x + SCROLL_STEP, game.y, view.x, view.y, SCROLL_STEP / TILE_X,
                   SCREEN_HEIGHT / TILE_Y);
      view.x = view.x - SCROLL_STEP;
    }

    actorlist *current = actors;
    while (current) {
      if (current->actor->type != PLAYER) {
        if (checkCollision(player, current->actor)) {
          printf("Collision!\n");
        }
      }
      current = current->next;
    }

    if (checkHitTiles(player)) {
      printf("Hit a tile!\n");
    }

    return;
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

void renderActors() {
  actorlist *current = actors;
  while (current != NULL) {
    Actor *a = current->actor;
    if (a->type == ENEMY) {
      a->x = a->x - 2;
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

  fire = initFireEffect(320, 120);
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
    SDL_Rect r = view;
    r.x = view.x * buffer->layer->parallaxx;
    r.y = view.y * buffer->layer->parallaxy;
    SDL_RenderCopy(gRenderer, buffer->buffer, &r, NULL);
    buffer = buffer->next;
  }

  renderActors();
  // move fire elsewhere
  SDL_RenderCopy(gRenderer, texFromFire(fire), NULL, NULL);
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
