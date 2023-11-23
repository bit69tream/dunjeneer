#pragma once

#include <stddef.h>
#include <unistd.h>
#include <raylib.h>

#include "types.h"

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

  bool vertical_first;
} Path;

typedef enum {
  TILE_NONE,
  TILE_FLOOR,
  TILE_WALL,
  TILE_VERTICAL_CLOSED_DOOR,
  TILE_HORIZONTAL_CLOSED_DOOR,
  TILE_VERTICAL_OPENED_DOOR,
  TILE_HORIZONTAL_OPENED_DOOR,
  LEVEL_TILE_COUNT,
} LevelTile;

typedef enum {
  OBJECT_NONE,
  OBJECT_ELEVATOR_DOWN,
  OBJECT_ELEVATOR_UP,
  LEVEL_OBJECT_COUNT,
} LevelObjectType;

typedef struct {
  LevelObjectType type;
  Point location;
} LevelObject;

#define LEVEL_WIDTH 200
#define LEVEL_HEIGHT 80

typedef LevelTile LevelMap[LEVEL_HEIGHT][LEVEL_WIDTH];

#define LEAFS_MAX 128
#define MIN_LEAF_SIZE 20

#define ROOMS_MAX LEAFS_MAX
#define PATHS_MAX LEAFS_MAX

#define OBJECTS_MAX 128

void generate_level(LevelMap *output_map, LevelObject objects[OBJECTS_MAX], size_t *objects_len, Point *player_location);
