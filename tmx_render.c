#include <SDL_image.h>
#include <SDL_render.h>
#include "tmx_render.h"
//#include "main.h"

void* SDL_tex_loader(const char *path) {
  return IMG_LoadTexture(gRenderer, path);
}

/*
void draw_sprite(void *image, unsigned int dx, unsigned int dy, float opacity, unsigned int flags) {
  SDL_Rect dest_rect;
  dest_rect.x = dx;
  dest_rect.y = dy;
  SDL_RenderCopy(gRenderer, (SDL_Texture*)image, NULL, &dest_rect);
}
*/

void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags) {
  SDL_Rect src_rect, dest_rect;
  src_rect.x = sx;
  src_rect.y = sy;
  src_rect.w = dest_rect.w = sw;
  src_rect.h = dest_rect.h = sh;
  dest_rect.x = dx;
  dest_rect.y = dy;
  SDL_RenderCopy(gRenderer, (SDL_Texture*)image, &src_rect, &dest_rect);
}

SDL_Texture* draw_layer(tmx_map *map, tmx_layer *layer, unsigned int x_offset_tiles, unsigned int y_offset_tiles, unsigned int x_num_tiles, unsigned int y_num_tiles) {

  SDL_Texture *layer_tex = SDL_CreateTexture(
      gRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET,
      x_num_tiles * TILE_X, y_num_tiles * TILE_Y);

  unsigned long i, j;
  unsigned int gid, x, y, w, h, flags;
  float op;
  tmx_tileset *ts;
  tmx_image *im;
  void* image;
  op = layer->opacity;
  SDL_SetRenderTarget(gRenderer, layer_tex);
  for (i=y_offset_tiles; i<y_offset_tiles + y_num_tiles; i++) {
    for (j=x_offset_tiles; j<x_offset_tiles + x_num_tiles && j < map->width; j++) {
      gid = (layer->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
      if (map->tiles[gid] != NULL) {
        ts = map->tiles[gid]->tileset;
        im = map->tiles[gid]->image;
        x  = map->tiles[gid]->ul_x;
        y  = map->tiles[gid]->ul_y;
        w  = ts->tile_width;
        h  = ts->tile_height;
        if (im) {
          image = im->resource_image;
        }
        else {
          image = ts->image->resource_image;
        }
        flags = (layer->content.gids[(i*map->width)+j]) & ~TMX_FLIP_BITS_REMOVAL;
        draw_tile(image, x, y, w, h, (j-x_offset_tiles)*ts->tile_width, (i-y_offset_tiles)*ts->tile_height, op, flags);
      }
    }
  }
  SDL_SetRenderTarget(gRenderer, NULL);
  return layer_tex;
}

/*
  void draw_image_layer(tmx_image *image) {
  SDL_Rect dim;
  dim.x = dim.y = 0;

  SDL_Texture *texture = (SDL_Texture*)image->resource_image; // Texture loaded by libTMX
  SDL_QueryTexture(texture, NULL, NULL, &(dim.w), &(dim.h));
  SDL_RenderCopy(gRenderer, texture, NULL, &dim);
}

void draw_polyline(double **points, double x, double y, int pointsc) {
  int i;
  for (i=1; i<pointsc; i++) {
    SDL_RenderDrawLine(gRenderer, x+points[i-1][0], y+points[i-1][1], x+points[i][0], y+points[i][1]);
  }
}

void draw_polygon(double **points, double x, double y, int pointsc) {
  draw_polyline(points, x, y, pointsc);
  if (pointsc > 2) {
    SDL_RenderDrawLine(gRenderer, x+points[0][0], y+points[0][1], x+points[pointsc-1][0], y+points[pointsc-1][1]);
  }
}

void draw_objects(tmx_object_group *objgr) {
  SDL_Rect rect;
  set_color(objgr->color);
  tmx_object *head = objgr->head;
  while (head) {
    if (head->visible) {
      if (head->obj_type == OT_SQUARE) {
        rect.x =     head->x;  rect.y =      head->y;
        rect.w = head->width;  rect.h = head->height;
        SDL_RenderDrawRect(gRenderer, &rect);
      }
      else if (head->obj_type  == OT_POLYGON) {
        draw_polygon(head->content.shape->points, head->x, head->y, head->content.shape->points_len);
      }
      else if (head->obj_type == OT_POLYLINE) {
        draw_polyline(head->content.shape->points, head->x, head->y, head->content.shape->points_len);
      }
      else if (head->obj_type == OT_ELLIPSE) {
      // TODO later
      }
    }
    head = head->next;
  }
}
*/

/*
void draw_all_layers(tmx_map *map, tmx_layer *layers) {
  while (layers) {
    if (layers->visible) {

      if (layers->type == L_GROUP) {
        draw_all_layers(map, layers->content.group_head); // recursive call
      }
//      else if (layers->type == L_OBJGR) {
//        draw_objects(layers->content.objgr); // Function to be implemented
//      }
//      else if (layers->type == L_IMAGE) {
//        draw_image_layer(layers->content.image); // Function to be implemented
//      }
      else if (layers->type == L_LAYER) {
        draw_layer(map, layers); // Function to be implemented
      }
    }
    layers = layers->next;
  }
}
*/
void set_color(int color) {
  tmx_col_bytes col = tmx_col_to_bytes(color);
  SDL_SetRenderDrawColor(gRenderer, col.r, col.g, col.b, col.a);
}

/*
void render_map(tmx_map *map) {
  set_color(map->backgroundcolor);
  SDL_RenderClear(gRenderer);
  draw_all_layers(map, map->ly_head);
}
*/
