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

#define PLAYER_VIEW_RADIUS 9
static_assert(PLAYER_VIEW_RADIUS % 2 == 1);

#define MAX_PLATER_INTERACTABLE_OFFSETS 512
extern Vector2I player_interactable_offsets[MAX_PLATER_INTERACTABLE_OFFSETS];
extern size_t player_interactable_offsets_len;

void process_player_movement(Player *player, LevelMap map);
void process_mouse(Player *player, LevelMap *map);

Color health_to_color(Player player);

void init_player(Player *player);
