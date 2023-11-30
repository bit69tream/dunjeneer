#pragma once

#include "types.h"
#include "level_generator.h"

#include <raylib.h>

typedef struct {
  Point location;
  Vector2I location_offset;
  Direction direction;

  size_t max_health;
  size_t health;
} Player;

typedef enum {
  ACTION_NONE,
  ACTION_OPEN,
  ACTION_CLOSE,
  ACTION_KICK,
  ACTION_PICK_UP,
  ACTION_EAT,
  ACTION_CLIMB,
  ACTION_COUNT,
} Action;

void process_player_movement(Player *player, LevelMap map);
void process_mouse(Player *player, LevelMap *map);

Color health_to_color(Player player);

void init_player(Player *player);
