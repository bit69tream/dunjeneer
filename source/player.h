#pragma once

#include "types.h"
#include "level_generator.h"

typedef struct {
  Point location;
  Vector2I location_offset;
  Direction direction;
} Player;

void process_player_movement(Player *player, LevelMap map);
