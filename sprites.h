#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib.h>
#include <SDL.h>
#include <SDL_image.h>

typedef struct Sprite {
  SDL_Texture *tex;
  int x, y;
  SDL_Rect rect;
} Sprite;
extern SDL_Renderer *gRenderer;

//void processSprites(xmlNode *rootElement, GHashTable *tileSet);
//GHashTable *loadSprites(char* fileName);
