#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdlib.h>

typedef struct Map {
  int rows,columns;
  char ***tile;
} Map;

Map *loadMap(char *fileName);
void readCSVData(xmlNode *rootElement, Map *map);


