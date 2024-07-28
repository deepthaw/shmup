#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <stdio.h>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 360;
const int SCALE = 1;
const int BG_LAYERS = 4;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

typedef struct Sprite {
  SDL_Texture *currentFrame;
  int x, y;
  SDL_Rect rect;
} Sprite;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

SDL_Texture *bgTexture = NULL;
SDL_Texture *spriteVicViper = NULL;
SDL_Texture *bgTiles = NULL;
SDL_Texture *backBufferSurface = NULL;

Sprite player;
char bgTileMap[80][45];
SDL_Rect view;

bool init();
bool loadMedia();
void close();
SDL_Surface *loadSurface(std::string path);
SDL_Texture *loadTexture(std::string path);

void processInput();
void update();
void render();
SDL_Texture *buildBackGround();

bool init() {
  bool success = true;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    success = false;
  } else {
    gWindow = SDL_CreateWindow("pew pew", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * SCALE,
                               SCREEN_HEIGHT * SCALE, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
      success = false;
    } else {
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

          bgTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB24,
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

  spriteVicViper = loadTexture("images/vicviper.png");
  if (spriteVicViper == NULL) {
    printf("Failed to load default image!\n");
    success = false;
  }

  bgTiles = loadTexture("images/tile00.png");
  if (bgTiles == NULL) {
    printf("Failed to load background tile!\n");
    success = false;
  }

  printf("loadMedia success\n");
  return success;
}

void close() {
  //  SDL_FreeSurface(player.currentFrame);
  //  player.currentFrame = NULL;

  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  SDL_Quit();
}

SDL_Surface *loadSurface(std::string path) {
  SDL_Surface *loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL Error: %s\n", path.c_str(),
           SDL_GetError());
  }
  return loadedSurface;
}

SDL_Texture *loadTexture(std::string path) {
  SDL_Texture *newTexture = NULL;
  SDL_Surface *loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image error: %s\n", path.c_str(),
           IMG_GetError());
  } else {
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
      printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(),
             SDL_GetError());
    }

    SDL_FreeSurface(loadedSurface);
  }

  return newTexture;
}

void processInput() {
  const Uint8 *keys = SDL_GetKeyboardState(NULL);

  if (keys[SDL_SCANCODE_UP]) {
    player.y--;
  }
  if (keys[SDL_SCANCODE_DOWN]) {
    player.y++;
  }
  if (keys[SDL_SCANCODE_LEFT]) {
    player.x--;
  }
  if (keys[SDL_SCANCODE_RIGHT]) {
    player.x++;
  }
}

void update() {
  view.x++;
  view.y = 0;
  view.w = 640;
  view.h = 360;
  player.rect.x = player.x;
  player.rect.y = player.y;
  return;
}

SDL_Texture *buildBackGround() {

  SDL_SetRenderTarget(gRenderer, bgTexture);
  struct SDL_Rect r;
  r.h = 8;
  r.w = 8;
  for (int i = 0; i != BG_LAYERS; i++) {
    for (int y = 0; y <= SCREEN_HEIGHT * 2; y += 8) {
      for (int x = 0; x <= SCREEN_WIDTH * 2; x += 8) {
        r.x = x;
        r.y = y;
        SDL_RenderCopy(gRenderer, bgTiles, NULL, &r);
      }
    }
  }
  SDL_SetRenderTarget(gRenderer, NULL);
  return bgTexture;
}

void render() {

  SDL_RenderClear(gRenderer);
  bgTexture = buildBackGround();
  SDL_RenderCopy(gRenderer, bgTexture, &view, NULL);
  SDL_RenderCopy(gRenderer, player.currentFrame, NULL, &player.rect);
  SDL_RenderPresent(gRenderer);
}

int main(int argc, char *args[]) {

  SDL_Event e;
  bool quit = false;
  uint64_t nextTic;
  if (!init()) {
    printf("Failed to initialize!\n");
  } else {
    if (!loadMedia()) {
      printf("Failed to load media!\n");
    } else {

      player.currentFrame = spriteVicViper;
      player.x = 320;
      player.y = 180;
      player.rect.w = 29;
      player.rect.h = 17;

      while (!quit) {
        while (SDL_PollEvent(&e) != 0)
        {
          if (e.type == SDL_QUIT)
          {
            quit = true;
          }
        }
        nextTic = SDL_GetTicks64() + SCREEN_TICKS_PER_FRAME;
        processInput();
        update();
        render();
        while (SDL_GetTicks64() <= nextTic);
      }
    }
  }
  close();

  return 0;
}
