#pragma once

#include <stddef.h>
#include <unistd.h>
#include <raylib.h>

typedef Rectangle Room;

typedef struct {
  size_t x;
  size_t y;
} Vector2I;

typedef Vector2I Point;

typedef struct {
  Rectangle rect;
  size_t left_leaf;
  size_t right_leaf;
  size_t parent;
} Leaf;

typedef struct {
  Point start;
  Point middle;
  Point end;

  bool vertical;
} Path;

typedef enum {
  TILE_NONE,
  TILE_FLOOR,
  TILE_WALL,
  TILE_DOOR,
} LevelTile;

#define LEVEL_WIDTH 200
#define LEVEL_HEIGHT 80

typedef LevelTile LevelMap[LEVEL_HEIGHT][LEVEL_WIDTH];

#define LEAFS_MAX 128
#define MIN_LEAF_SIZE 20

#define ROOMS_MAX LEAFS_MAX
#define PATHS_MAX LEAFS_MAX

void generate_level(LevelMap *output_map);
