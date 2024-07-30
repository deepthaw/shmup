#include "sprites.h"
#include "glib.h"
#include <SDL_image.h>
#include <string.h>

void processSprites(xmlNode *rootElement, GHashTable *spriteHash) {
  xmlNode *currentNode = NULL;
  for (currentNode = rootElement; currentNode;
       currentNode = currentNode->next) {
    if (currentNode->type == XML_ELEMENT_NODE &&
        xmlStrcmp(currentNode->name, (const xmlChar *)"tile") == 0) {
      xmlChar *id = xmlGetProp(currentNode, (const xmlChar *)"id");
      if (id) {
        Sprite *s = malloc(sizeof(Sprite));
        g_hash_table_insert(spriteHash, g_strdup(id), s);
        xmlNode *childNode = NULL;
        for (childNode = currentNode->children; childNode;
             childNode = childNode->next) {
          if (childNode->type == XML_ELEMENT_NODE &&
              xmlStrcmp(childNode->name, (const xmlChar *)"image") == 0) {
            xmlChar *source = xmlGetProp(childNode, (const xmlChar *)"source");
            xmlChar *widthStr = xmlGetProp(childNode, (const xmlChar *)"width");
            xmlChar *heightStr =
                xmlGetProp(childNode, (const xmlChar *)"height");
            s->rect.w = widthStr ? atoi((const char *)widthStr) : 0;
            s->rect.h = heightStr ? atoi((const char *)heightStr) : 0;
            s->tex = IMG_LoadTexture(gRenderer, source);
          }
        }

        xmlFree(id);
      }
    }
    processSprites(currentNode->children, spriteHash);
  }
}

GHashTable *loadSprites(char *fileName) {
  LIBXML_TEST_VERSION
  GHashTable *hash_table = g_hash_table_new(g_str_hash, g_str_equal);

  xmlDoc *document = xmlReadFile(fileName, NULL, 0);
  if (document == NULL) {
    printf("Error: could not parse file.\n");
    return NULL;
  }

  xmlNode *rootElement = xmlDocGetRootElement(document);

  processSprites(rootElement, hash_table);
  xmlFreeDoc(document);

  xmlCleanupParser();

  return hash_table;
}
