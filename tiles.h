#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib.h>
#include <SDL.h>
#include <SDL_image.h>

typedef struct Tile {
  char fileName[256];
  SDL_Texture* tex;
  int w;
  int h;
} Tile;

extern SDL_Renderer *gRenderer;

void processTiles(xmlNode *rootElement, GHashTable *tileSet);
GHashTable *loadTiles(char* fileName);
