#pragma once

#include <stddef.h>
#include <sys/types.h>
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

  TILE_ELEVATOR_DOWN,
  TILE_ELEVATOR_UP,

  TILE_VERTICAL_CLOSED_DOOR,
  TILE_HORIZONTAL_CLOSED_DOOR,
  TILE_VERTICAL_LOCKED_DOOR,
  TILE_HORIZONTAL_LOCKED_DOOR,
  TILE_VERTICAL_OPENED_DOOR,
  TILE_HORIZONTAL_OPENED_DOOR,

  TILE_GROUND,
  TILE_HILL,
  TILE_MOUNTAIN,

  LEVEL_TILE_COUNT,
} LevelTileType;

typedef struct {
  LevelTileType type;
  ssize_t durability;
} LevelTile;

typedef enum {
  OBJECT_NONE,
  LEVEL_OBJECT_COUNT,
} LevelObjectType;

typedef struct {
  LevelObjectType type;
  Point location;
} LevelObject;

typedef enum {
  LEVEL_NONE,
  LEVEL_SURFACE,
  LEVEL_DUNGEON,
  LEVEL_TYPE_COUNT,
} LevelType;

#define LEVEL_WIDTH 200
#define LEVEL_HEIGHT 80

typedef struct {
  LevelType type;
  LevelTileType floor;
  LevelTile map[LEVEL_HEIGHT][LEVEL_WIDTH];
} LevelMap;

#define LEAFS_MAX 128
#define MIN_LEAF_SIZE 20

#define ROOMS_MAX LEAFS_MAX
#define PATHS_MAX LEAFS_MAX

#define OBJECTS_MAX 128

#define DURABILITY_MAX (60 * 10)

void generate_level(LevelMap *output_map,
                    LevelObject objects[OBJECTS_MAX], size_t *objects_len,
                    Point *player_location,
                    LevelType type);

bool is_tile_solid(LevelTileType tile);

bool can_be_drilled(LevelTileType tile);
