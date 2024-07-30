#include "map.h"
#include "strdup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to read CSV data from a tile layer
void readCSVData(xmlNode *rootElement, Map *map) {
  xmlNode *currentNode = NULL;
  xmlChar *csvData = NULL;
  int width, height;
  // Traverse through the XML nodes
  for (currentNode = rootElement; currentNode;
       currentNode = currentNode->next) {
    // Check if the node is a "layer" element
    if (currentNode->type == XML_ELEMENT_NODE &&
        xmlStrcmp(currentNode->name, (const xmlChar *)"layer") == 0) {
      // Retrieve the "width" and "height" attributes
      xmlChar *widthStr = xmlGetProp(currentNode, (const xmlChar *)"width");
      xmlChar *heightStr = xmlGetProp(currentNode, (const xmlChar *)"height");

      width = widthStr ? atoi((const char *)widthStr) : 0;
      height = heightStr ? atoi((const char *)heightStr) : 0;
      // Free the width and height attribute strings
      if (widthStr)
        xmlFree(widthStr);
      if (heightStr)
        xmlFree(heightStr);

      // Traverse through the children of the "layer" element
      xmlNode *childNode = NULL;
      for (childNode = currentNode->children; childNode;
           childNode = childNode->next) {
        // Check if the node is a "data" element and has a "csv" encoding
        // attribute
        if (childNode->type == XML_ELEMENT_NODE &&
            xmlStrcmp(childNode->name, (const xmlChar *)"data") == 0) {
          xmlChar *encoding =
              xmlGetProp(childNode, (const xmlChar *)"encoding");
          if (encoding && xmlStrcmp(encoding, (const xmlChar *)"csv") == 0) {
            // The content of the "data" element contains the CSV data
            csvData = xmlNodeGetContent(childNode);
          }
          if (encoding) {
            xmlFree(encoding);
          }
        }
      }
    }
    // Recursively process child nodes
    readCSVData(currentNode->children, map);
  }
  if (csvData) {
    char ***data = malloc(height * sizeof(char **));
    for (int i = 0; i != height; i++) {
      data[i] = malloc(width * sizeof(char *));
        for (int j = 0; j != width; j++) {
          data[i][j] = malloc(sizeof(char));
        }
    }

    char *tok = strtok(csvData, ",\n");
    for (int i = 0; i != height; i++) {
      for (int j = 0; j != width; j++) {
        data[i][j] = strdup(tok);
        tok = strtok(NULL, ",\n");
      }
    }
    map->rows = height;
    map->columns = width;
    map->tile = data;
  }
}

Map *loadMap(char *fileName) {
  // Map *loadMap(char *fileName) {
  //  int main(int argc, char **argv) {
  //   Initialize the library and check potential ABI mismatches
  LIBXML_TEST_VERSION
  Map *map = malloc(sizeof(Map));
  // Parse the file and get the DOM
  xmlDoc *document = xmlReadFile(fileName, NULL, 0);
  if (document == NULL) {
    printf("Error: could not parse file.\n");
    return 0;
  }

  // Get the root element node
  xmlNode *rootElement = xmlDocGetRootElement(document);

  // Read the CSV data from the "layer" elements
  readCSVData(rootElement, map);

  // Free the document
  xmlFreeDoc(document);

  // Cleanup function for the XML library
  xmlCleanupParser();

  return map;
}
