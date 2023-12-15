#include "player.h"
#include "font.h"
#include "level_generator.h"
#include "rand.h"
#include "raylib.h"
#include "raymath.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

void process_player_movement(Player *player, const Level *map) {
  if (ui_state.type != UI_STATE_NONE) {
    return;
  }

  if (is_action_key_down(KEYBIND_ACTION_MOVE_UP)) {
    if ((player->direction == DIRECTION_NONE ||
         player->direction == DIRECTION_LEFT ||
         player->direction == DIRECTION_RIGHT) &&
        (player->location.y != 0) &&
        (!is_tile_solid(map->map[player->location.y - 1][player->location.x].type))) {
      player->direction |= DIRECTION_UP;
      player->location_offset.y = (GLYPH_HEIGHT + GLYPH_GAP);
      player->location.y -= 1;
    }
  }

  if (is_action_key_down(KEYBIND_ACTION_MOVE_LEFT)) {
    if ((player->direction == DIRECTION_NONE ||
         player->direction == DIRECTION_DOWN ||
         player->direction == DIRECTION_UP) &&
        (player->location.x != 0) &&
        (!is_tile_solid(map->map[player->location.y][player->location.x - 1].type))) {
      player->direction |= DIRECTION_LEFT;
      player->location_offset.x = (GLYPH_WIDTH + GLYPH_GAP);
      player->location.x -= 1;
    }
  }

  if (is_action_key_down(KEYBIND_ACTION_MOVE_DOWN)) {
    if ((player->direction == DIRECTION_NONE ||
         player->direction == DIRECTION_LEFT ||
         player->direction == DIRECTION_RIGHT) &&
        (player->location.y != LEVEL_HEIGHT - 1) &&
        (!is_tile_solid(map->map[player->location.y + 1][player->location.x].type))) {
      player->direction |= DIRECTION_DOWN;
      player->location_offset.y = -(GLYPH_HEIGHT + GLYPH_GAP);
      player->location.y += 1;
    }
  }

  if (is_action_key_down(KEYBIND_ACTION_MOVE_RIGHT)) {
    if ((player->direction == DIRECTION_NONE ||
         player->direction == DIRECTION_UP ||
         player->direction == DIRECTION_DOWN) &&
        (player->location.x != LEVEL_WIDTH - 1) &&
        (!is_tile_solid(map->map[player->location.y][player->location.x + 1].type))) {
      player->direction |= DIRECTION_RIGHT;
      player->location_offset.x = -(GLYPH_WIDTH + GLYPH_GAP);
      player->location.x += 1;
    }
  }

  if (player->direction != DIRECTION_NONE) {
    if (player->location_offset.x == 0) {
      player->direction = player->direction & (unsigned int)(~DIRECTION_LEFT);
      player->direction = player->direction & (unsigned int)(~DIRECTION_RIGHT);
    }

    if (player->location_offset.y == 0) {
      player->direction = player->direction & (unsigned int)(~DIRECTION_UP);
      player->direction = player->direction & (unsigned int)(~DIRECTION_DOWN);
    }

    if (player->direction & DIRECTION_LEFT ||
        player->direction & DIRECTION_RIGHT) {
      if (player->location_offset.x < 0) {
        player->location_offset.x += 1;
      } else {
        player->location_offset.x -= 1;
      }
    }

    if (player->direction & DIRECTION_UP ||
        player->direction & DIRECTION_DOWN) {
      if (player->location_offset.y < 0) {
        player->location_offset.y += 1;
      } else {
        player->location_offset.y -= 1;
      }
    }
  }

  player->location.x = CLAMP(0, LEVEL_WIDTH - 1, player->location.x);
  player->location.y = CLAMP(0, LEVEL_HEIGHT - 1, player->location.y);
}

Action get_action_from_menu(void) {
  Point mouse_position = mouse_in_world();

  for (size_t i = 0; i < SIZE_OF(action_menu_offsets); i++) {
    Point position = {
      .x = (size_t)((ssize_t)ui_state.action_tile_location.x + action_menu_offsets[i].x),
      .y = (size_t)((ssize_t)ui_state.action_tile_location.y + action_menu_offsets[i].y),
    };

    if (mouse_position.x == position.x &&
        mouse_position.y == position.y) {
      return (Action)i;
    }
  }

  return ACTION_NONE;
}

#define TILE_FROM_LOCATION(map, location) ((map)[(location).y][(location.x)])

void action_open(Player *player, Level *map, Point location) {
  (void) player;

  LevelTile *tile = &TILE_FROM_LOCATION(map->map, location);

  switch (tile->type) {
  case TILE_HORIZONTAL_CLOSED_DOOR: tile->type = TILE_HORIZONTAL_OPENED_DOOR; break;
  case TILE_VERTICAL_CLOSED_DOOR: tile->type = TILE_VERTICAL_OPENED_DOOR; break;
  default: return;
  }
}

void action_close(Player *player, Level *map, Point location) {
  (void) player;

  LevelTile *tile = &TILE_FROM_LOCATION(map->map, location);

  switch (tile->type) {
  case TILE_HORIZONTAL_OPENED_DOOR: tile->type = TILE_HORIZONTAL_CLOSED_DOOR; break;
  case TILE_VERTICAL_OPENED_DOOR: tile->type = TILE_VERTICAL_CLOSED_DOOR; break;
  default: return;
  }
}

void action_climb(Player *player,
                  Level *map, Point location,
                  size_t *current_level, size_t levels_max) {
  (void) player;

  LevelTile *tile = &TILE_FROM_LOCATION(map->map, location);

  switch (tile->type) {
  case TILE_ELEVATOR_DOWN: {
    *current_level += 1;
    if (*current_level >= (levels_max - 1)) {
      *current_level = levels_max - 1;
    }
  } break;
  case TILE_ELEVATOR_UP: {
    if (*current_level == 0) {
      assert(false && "there isn't supposed to be an elevator up on the surface");
    }

    *current_level -= 1;
  } break;
  default: return;
  }
}

void apply_action(Player *player,
                  Level *map, Point location,
                  Action action,
                  size_t *current_level, size_t levels_max) {
  switch (action) {
  case ACTION_NONE: assert(false && "cannot apply ACTION_NONE");
  case ACTION_OPEN: action_open(player, map, location); return;
  case ACTION_CLOSE: action_close(player, map, location); return;
  case ACTION_PICK_UP: assert(false && "TODO");
  case ACTION_CLIMB: action_climb(player, map, location, current_level, levels_max); return;
  case ACTION_COUNT: assert(false && "( ._.)");
  }
}

void process_mouse(Player *player, Level *map, size_t *current_level, size_t levels_max) {
  player->is_drilling = is_action_key_down(KEYBIND_ACTION_FIRE);

  Point mouse_position = mouse_in_world();

  if (player->is_drilling) {
    ssize_t dx = ((ssize_t)player->location.x + player->drill_offset.x);
    ssize_t dy = ((ssize_t)player->location.y + player->drill_offset.y);

    if ((dx >= 0 && dx < LEVEL_WIDTH) &&
        (dy >= 0 && dy < LEVEL_HEIGHT) &&
        can_be_drilled(map->map[dy][dx].type)) {
      map->map[dy][dx].durability -= 1;
      if (map->map[dy][dx].durability <= 0) {
        map->map[dy][dx].type = map->floor;
      }
      map->map[dy][dx].durability = CLAMP(0, DURABILITY_MAX, map->map[dy][dx].durability);
    }

  }

  if (is_action_key_pressed(KEYBIND_ACTION_ACTION_MENU)) {
    assert(ui_state.type == UI_STATE_NONE);

    if (!map->map[mouse_position.y][mouse_position.x].is_visible) {
      return;
    }

    ui_state.type = UI_STATE_ACTION_MENU;
    ui_state.action_tile_location = mouse_position;

    assert(ui_state.type == UI_STATE_ACTION_MENU);
  }

  if (is_action_key_released(KEYBIND_ACTION_ACTION_MENU) && ui_state.type == UI_STATE_ACTION_MENU) {
    Action action = get_action_from_menu();
    assert(action >= ACTION_NONE && action < ACTION_COUNT);

    if (action != ACTION_NONE) {
      /* TODO: play some sound */
      apply_action(player,
                   map, ui_state.action_tile_location,
                   action,
                   current_level, levels_max);
    }

    ui_state.type = UI_STATE_NONE;
  }
}

Color health_to_color(Player player) {
  float health_percentage = (float)player.health / (float)player.max_health;

  Color dead = CLITERAL(Color) {
    .r = 214,
    .g = 19,
    .b = 48,
    .a = 255,
  };
  Color alive = GREEN;

  return color_interpolate(dead, alive, health_percentage);
}

void trace_ray(Vector2 from, Vector2 to,
               Level *map) {
  Vector2 dir = Vector2Normalize(Vector2Subtract(to, from));

  Vector2 unit_step_size = (Vector2) {
    .x = sqrtf(1 + powf(dir.y / dir.x, 2)),
    .y = sqrtf(1 + powf(dir.x / dir.y, 2)),
  };

  Vector2I check = (Vector2I) {
    .x = (ssize_t) from.x,
    .y = (ssize_t) from.y,
  };
  Vector2 ray_length = (Vector2) {0, 0};
  Vector2I step = (Vector2I) {0, 0};

  if (dir.x < 0) {
    step.x = -1;
    ray_length.x = (from.x - (float)(check.x)) * unit_step_size.x;
  } else {
    step.x = 1;
    ray_length.x = ((float)(check.x + 1) - from.x) * unit_step_size.x;
  }

  if (dir.y < 0) {
    step.y = -1;
    ray_length.y = (from.y - (float)(check.y)) * unit_step_size.y;
  } else {
    step.y = 1;
    ray_length.y = ((float)(check.y + 1) - from.y) * unit_step_size.y;
  }

  float distance = 0;
  while (distance < PLAYER_MAX_VIEW_DISTANCE) {
    if (ray_length.x < ray_length.y) {
      check.x += step.x;
      distance = ray_length.x;
      ray_length.x += unit_step_size.x;
    } else {
      check.y += step.y;
      distance = ray_length.y;
      ray_length.y += unit_step_size.y;
    }

    if ((check.x < 0 || check.x >= LEVEL_WIDTH) ||
        (check.y < 0 || check.y >= LEVEL_HEIGHT)) {
      break;
    }

    LevelTile tile = map->map[check.y][check.x];

    map->map[check.y][check.x].is_visible = true;

    if (is_tile_solid(tile.type)) {
      break;
    }
  }
}

void trace_rays_for_fov(Player player,
                        Level *map) {
  for (size_t yi = 0; yi < LEVEL_HEIGHT; yi++) {
    for (size_t xi = 0; xi < LEVEL_WIDTH; xi++) {
      map->map[yi][xi].is_visible = false;
    }
  }

  Vector2 from = (Vector2) {
    .x = (float)player.location.x + 0.5f,
    .y = (float)player.location.y + 0.5f,
  };

  for (ssize_t yi = -25; yi < 25; yi++) {
    for (ssize_t xi = -25; xi < 25; xi++) {
      Vector2 to = (Vector2) {
        .x = from.x - (float)(xi),
        .y = from.y - (float)(yi),
      };

      trace_ray(from, to, map);
    }
  }

  map->map[player.location.y][player.location.x].is_visible = true;
}

void init_player(Player *player) {
  player->location = (Point){0};
  player->location_offset = (Vector2I){0};
  player->direction = DIRECTION_NONE;

  #define PLAYER_INITIAL_MAX_HEALTH 50

  player->health =
    player->max_health =
    PLAYER_INITIAL_MAX_HEALTH;
}

void update_drill_position(Player *player) {
  Point m = mouse_in_world();
  Vector2 to = (Vector2) {
    .x = (float) m.x,
    .y = (float) m.y,
  };
  Vector2 from = (Vector2) {
    .x = (float) player->location.x,
    .y = (float) player->location.y,
  };
  Vector2 dir = Vector2Normalize(Vector2Subtract(to, from));

  player->drill_offset = (Vector2I) {
    .x = (ssize_t)roundf(dir.x),
    .y = (ssize_t)roundf(dir.y),
  };
}
