#include "main.h"
#include "fire.h"
#include "sprites.h"
#include "tmx.h"
#include "tmx_render.h"
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <mikmod.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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
bool initSound();
Sprite *createSprite(char *name, int type);
void addSprite(spritelist **sprites, Sprite *s);
void *playMusic(void *mod);
void checkSpawns(int cell_x);
void spawnEnemyAt(int32_t GID, int cell_x, int cell_y);


tmx_map *map;
gameData game;
renderList *buffers;
spritelist *sprites = NULL;
Sprite *player;
MODULE *module;
SDL_Renderer *gRenderer = NULL;
framebuffer *fire;

bool initSound() {
  bool success = true;
  MikMod_RegisterAllDrivers();
  MikMod_RegisterAllLoaders();
  md_mode |= DMODE_SOFT_MUSIC;
  if (MikMod_Init("")) {
    fprintf(stderr, "Could not initialize sound, reason %s\n",
            MikMod_strerror(MikMod_errno));
    success = false;
  }
  return success;
}

void *playMusic(void *mod) {
  module = Player_Load(mod, 64, 0);
  if (module) {
    Player_Start(module);
    while (Player_Active()) {
      usleep(10000);
      MikMod_Update();
    }
  }
}

void scrollScreen(tmx_map *map, int x, int y, int x_offsetpx, int y_offsetpx,
                  int x_size, int y_size);
void initializeBackground();

int screen_height_tiles = SCREEN_HEIGHT / TILE_Y;
int screen_width_tiles = SCREEN_WIDTH / TILE_X;

int player_holdup, player_holddown, player_holdright, player_holdleft = 0;
bool SCROLL_STOP = false;
bool PAUSED = true;

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
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
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
  Sprite *enemy = createSprite("E_ROLLER", ENEMY);
  int x = cell_x * TILE_X + (cell_x * TILE_X % SCREEN_WIDTH);
  int y = cell_y * TILE_Y;
  enemy->rect.x = x;
  enemy->rect.y = y;
  addSprite(&sprites, enemy);
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

Sprite *createSprite(char *name, int type) {
  printf("Trying to create %s\n", name);
  Sprite *s = NULL;
  s = malloc(sizeof(Sprite));
  tmx_tileset *tileset = tmx_find_tileset_by_name(map, name)->tileset;
  int tilecount = tileset->tilecount;
  tmx_tile *tiles = tileset->tiles;
  s->animtex = malloc(NUMSTATES * sizeof(animateLoop *));
  for (int i = 0; i != NUMSTATES; i++) {
    const char *currentstate = stateTable[i];
    char *n = calloc(strlen(currentstate) + 1, sizeof(char));
    s->animtex[i] = NULL;
    strcpy(n, currentstate);
    for (int j = 0; j != 10; j++) {
      animateLoop *current;
      n[strlen(currentstate)] = j + '0';
      for (int k = 0; k != tilecount; k++) {
        tmx_property prop = *tmx_get_property(tiles[k].properties, "state");
        if (strcmp(prop.value.string, n) == 0) {
          animateLoop *new = malloc(sizeof(animateLoop));
          new->frame = malloc(sizeof(spriteTex));
          new->frame->tex = IMG_LoadTexture(gRenderer, tiles[k].image->source);
          new->frame->rect.h = tiles[k].height;
          new->frame->rect.w = tiles[k].width;
          new->tics = 5;
          new->next = NULL;

          if (s->animtex[i] == NULL) {
            s->animtex[i] = new;
          } else {
            current = s->animtex[i];
            while (current->next != NULL) {
              current = current->next;
            }
            current->next = new;
          }
        }
      }
    }
    if (s->animtex[i] != NULL) {
      animateLoop *tmp = s->animtex[i];
      while (tmp->next != NULL) {
        tmp = tmp->next;
      }
      tmp->next = s->animtex[i];
    }
  }
  s->type = type;
  setSpriteState(s, SPNEUTRAL);
  return s;
}

void addSprite(spritelist **sprites, Sprite *s) {
  spritelist *new = calloc(1, sizeof(spritelist));
  if (new == NULL) {
    printf("failed to allocate memory!\n");
    exit(EXIT_FAILURE);
  }
  new->sprite = s;
  new->next = NULL;
  if (*sprites == NULL) {
    *sprites = new;
  } else {
    spritelist *node = *sprites;
    while (node->next != NULL) {
      node = node->next;
    }
    node->next = new;
  }
}

void updateSprite(Sprite *s) {
  int state = s->state;
 
  if (s->elapsed++ > s->animtex[state]->tics) {
  s->animtex[state] = s->animtex[state]->next;
  s->tex = s->animtex[state]->frame->tex;
  s->elapsed = 0;
  }
}

void renderSprites() {
  spritelist *current = sprites;
  int i = 0;
  while (current != NULL) {
    Sprite *s = current->sprite;
    if (s->type == ENEMY) {
      s->rect.x = s->rect.x - 2;
    }
    printf("rendering %u\n",i++);
    SDL_RenderCopy(gRenderer, s->tex, NULL, &s->rect);
    updateSprite(s);
    current = current->next;
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

  if (!initSound()) {
    printf("Failed to initialize sound!\n");
    return 1;
  }

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
  srand(time(NULL));
  fire = initFireEffect(320, 120);
  initializeBackGround();
  initializeView(&view);
  player = createSprite("P_VIPER", PLAYER);
  Sprite *badguy = createSprite("E_ROLLER", ENEMY);
  addSprite(&sprites, player);
  addSprite(&sprites, badguy);
  badguy->rect.x = 160;
  badguy->rect.y = 120;
  player->rect.x = 0;
  player->rect.y = 120;
  printf("done loading\n");
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
