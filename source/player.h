#pragma once

#include "types.h"
#include "level_generator.h"

#include <assert.h>
#include <raylib.h>
#include <stddef.h>

typedef struct {
  Point location;
  Vector2I location_offset;
  Direction direction;

  size_t max_health;
  size_t health;

  bool is_drilling;
  Vector2I drill_offset;
} Player;

typedef enum {
  ACTION_NONE,
  ACTION_OPEN,
  ACTION_CLOSE,
  ACTION_PICK_UP,
  ACTION_CLIMB,
  ACTION_COUNT,
} Action;

#define PLAYER_MAX_VIEW_DISTANCE 20

void process_player_movement(Player *player, const Level *map);
void process_mouse(Player *player, Level *map);

Color health_to_color(Player player);

void init_player(Player *player);

void trace_rays_for_fov(Player player, Level *map);

void update_drill_position(Player *player);
